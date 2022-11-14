#include "Webserver.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <cerrno>
#include <string>

#include "../utils/color.hpp"
#include "../utils/timestamp.hpp"
#include "Address.hpp"
#include "EventNotificationInterface.hpp"

namespace core {

Webserver::Webserver(const std::vector<config::Server> &v_server)
    : _eni(_m_socket),
      _v_server(v_server),
      _used_connections(0),
      _max_connections(MAX_CONNECTIONS) {
    _v_connection.resize(_max_connections);

    // Create sockets
    typedef std::vector<config::Server>::const_iterator server_it_t;
    typedef std::vector<Address>::const_iterator        listen_it_t;

    std::vector<Address> v_added_listens;
    for (server_it_t it_server = _v_server.begin(); it_server != _v_server.end(); ++it_server) {
        for (listen_it_t it_listen = it_server->v_listen.begin();
             it_listen != it_server->v_listen.end(); ++it_listen) {
            if (std::find(v_added_listens.begin(), v_added_listens.end(), *it_listen) ==
                v_added_listens.end()) {
                v_added_listens.push_back(*it_listen);
                Socket socket(it_listen->addr, it_listen->port);
                _m_socket.insert(std::make_pair(socket.fd(), socket));
                _eni.add_event(socket.fd(), EVFILT_READ);
            }
        }
    }
}

Webserver::~Webserver() {}

void Webserver::run() {
#if PRINT_LEVEL > 0
    std::cout << utils::COLOR_CY_1 << "Webserver running 🚀" << utils::COLOR_NO << std::endl;
#endif
    while (42) {
        try {
            int num_events = _eni.poll_events();
            if (num_events == -1)
                throw std::runtime_error("poll_events: " + std::string(strerror(errno)));
            for (int i = 0; i < num_events; i++) {
                try {
                    // Kevent error
                    if (_eni.events[i].flags & EV_ERROR) {
                        throw std::runtime_error("kevent: " + std::string(strerror(errno)));
                    }

                    // New connection on listen socket
                    const core::Socket *socket = _eni.find_socket(_eni.events[i].ident);
                    if (socket) {
                        _accept_connection(*socket);
                        continue;
                    }

                    // New event on cgi fd
                    core::CgiHandler *cgi = _eni.find_cgi(_eni.events[i].ident);
                    if (cgi) {
                        if (_eni.events[i].filter == EVFILT_READ) {
                            if (_eni.events[i].data <= 0 && _eni.events[i].flags & EV_EOF) {
                                cgi->eof_read_write(_eni);
                            } else if (_eni.events[i].data > 0)
                                cgi->read(_eni, _eni.events[i].data);
                        } else if (_eni.events[i].filter == EVFILT_WRITE) {
                            if (_eni.events[i].flags & EV_EOF)
                                cgi->eof_read_write(_eni);
                            else if (_eni.events[i].data > 0)
                                cgi->write(_eni, _eni.events[i].data);
                        }
                        continue;
                    }

                    // Event on established connection
                    if (_eni.events[i].filter == EVFILT_TIMER) {
                        _timeout_connection(_eni.events[i].ident);
                    } else if (_eni.events[i].filter == EVFILT_READ) {
                        if (_eni.events[i].data <= 0 && _eni.events[i].flags & EV_EOF) {
                            _close_connection(_eni.events[i].ident);
                        } else if (_eni.events[i].data > 0) {
                            _receive(_eni.events[i].ident, _eni.events[i].data);
                        }
                    } else if (_eni.events[i].filter == EVFILT_WRITE) {
                        if (_eni.events[i].flags & EV_EOF) {
                            _close_connection(_eni.events[i].ident);
                        } else if (_eni.events[i].data > 0) {
                            _send(_eni.events[i].ident, _eni.events[i].data);
                        }
                    }
                } catch (const std::exception &e) {
                    _close_connection(_eni.events[i].ident);
                    std::cerr << "[";
                    utils::print_timestamp(std::cerr);
                    std::cerr << "]: " << e.what() << '\n';
                } catch (...) {
                    _close_connection(_eni.events[i].ident);
                    std::cerr << "[";
                    utils::print_timestamp(std::cerr);
                    std::cerr << "]: "
                              << "Unknown error\n";
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "[";
            utils::print_timestamp(std::cerr);
            std::cerr << "]: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "[";
            utils::print_timestamp(std::cerr);
            std::cerr << "]: "
                      << "Unknown error\n";
        }
    }
}

void Webserver::_accept_connection(const Socket &socket) {
    struct ::sockaddr_in accept_addr;
    socklen_t            accept_addr_len = sizeof(::sockaddr_in);
    Address              client_addr;

    int accept_fd = ::accept(socket.fd(), (struct sockaddr *)&accept_addr, &accept_addr_len);
    if (accept_fd == -1) {
        throw std::runtime_error("accept: " + std::string(strerror(errno)));
    }

    if (fcntl(accept_fd, F_SETFL, O_NONBLOCK) == -1) {
        close(accept_fd);
        throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    }

    int optval = 1;
    if (setsockopt(accept_fd, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) < 0) {
        close(accept_fd);
        throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
    }

    client_addr.addr = accept_addr.sin_addr.s_addr;
    client_addr.port = accept_addr.sin_port;

    int error =
        (_eni.add_timer(accept_fd, TIMEOUT_TIME) || _eni.add_event(accept_fd, EVFILT_READ) ||
         _eni.add_event(accept_fd, EVFILT_WRITE) || _eni.disable_event(accept_fd, EVFILT_WRITE));

    if (error) {
        _eni.delete_event(accept_fd, EVFILT_TIMER);
        _eni.delete_event(accept_fd, EVFILT_READ);
        _eni.delete_event(accept_fd, EVFILT_WRITE);
        close(accept_fd);
        throw std::runtime_error("eni: " + std::string(strerror(errno)));
    }

    if (_used_connections >= _max_connections) {
        for (std::vector<Connection>::iterator it = _v_connection.begin();
             it != _v_connection.end(); ++it) {
            if (!it->is_active()) {
                _close_connection(it);
                it->init(accept_fd, client_addr, socket.addr());
                return;
            }
        }
        _eni.delete_event(accept_fd, EVFILT_TIMER);
        _eni.delete_event(accept_fd, EVFILT_READ);
        _eni.delete_event(accept_fd, EVFILT_WRITE);
        close(accept_fd);
        throw std::runtime_error("connection limit reached");
    } else {
        std::vector<Connection>::iterator it =
            std::find(_v_connection.begin(), _v_connection.end(), -1);
        it->init(accept_fd, client_addr, socket.addr());
        _used_connections++;
    }
}

void Webserver::_receive(int fd, size_t data_len) {
    std::vector<Connection>::iterator conn_it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);

    try {
        conn_it->receive(data_len);
        if (_eni.add_timer(fd, TIMEOUT_TIME))
            throw std::runtime_error("eni: " + std::string(strerror(errno)));
        conn_it->parse_request(_v_server);
        if (conn_it->is_request_done()) {
            if (_eni.disable_event(fd, EVFILT_READ) || _eni.enable_event(fd, EVFILT_WRITE)) {
                throw std::runtime_error("eni: " + std::string(strerror(errno)));
            }
            conn_it->build_response(_eni);
        }
    } catch (...) {
        _close_connection(conn_it);
        throw;
    }
}

void Webserver::_send(int fd, size_t max_len) {
    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);
    try {
        if (it->send_response(_eni, max_len)) {
            if (_eni.add_timer(fd, TIMEOUT_TIME))
                throw std::runtime_error("eni: " + std::string(strerror(errno)));
        }
        if (it->is_response_done()) {
            if (it->should_close()) {
                _close_connection(it);
                return;
            }
            it->reinit();
            it->parse_request(_v_server);
            if (it->is_request_done()) {
                it->build_response(_eni);
                return;
            }

            if (_eni.disable_event(it->fd(), EVFILT_WRITE) ||
                _eni.enable_event(it->fd(), EVFILT_READ)) {
                throw std::runtime_error("eni: " + std::string(strerror(errno)));
            }
        }
    } catch (...) {
        _close_connection(it);
        throw;
    }
}

void Webserver::_close_connection(int fd) {
    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);
    if (it != _v_connection.end()) {
        _close_connection(it);
    }
}

void Webserver::_close_connection(std::vector<Connection>::iterator it) {
    _eni.delete_event(it->fd(), EVFILT_TIMER);
    _eni.delete_event(it->fd(), EVFILT_READ);
    _eni.delete_event(it->fd(), EVFILT_WRITE);
    it->destroy(_eni);
    _used_connections--;
}

void Webserver::_timeout_connection(int fd) {
#if PRINT_LEVEL > 0
    std::cout << utils::COLOR_CY << "[Timeout] " << utils::COLOR_NO;
#endif
    _close_connection(fd);
}

}  // namespace core
