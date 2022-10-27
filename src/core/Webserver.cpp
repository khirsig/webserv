#include "Webserver.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <cerrno>
#include <string>

namespace core {

Webserver::Webserver(std::vector<config::Server> v_server, size_t max_connections)
    : _used_connections(0),
      _max_connections(max_connections),
      _v_server(v_server),
      _read_buf_size(MAX_PIPE_SIZE) {
    _v_connection.resize(_max_connections);
    _read_buf = new char[_read_buf_size];
}

Webserver::~Webserver() { delete[] _read_buf; }

void Webserver::accept_connection(const Socket &socket) {
    struct ::sockaddr_in accept_addr;
    socklen_t            accept_addr_len = sizeof(::sockaddr_in);
    Address              client_addr;

    int accept_fd = ::accept(socket.fd, (struct sockaddr *)&accept_addr, &accept_addr_len);
    if (accept_fd == -1) {
        throw std::runtime_error("accept: " + std::string(strerror(errno)));
    }

    if (fcntl(accept_fd, F_SETFL, O_NONBLOCK) == -1) {
        ::close(accept_fd);
        throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    }

    client_addr.addr = accept_addr.sin_addr.s_addr;
    client_addr.port = accept_addr.sin_port;

    int error =
        (_eni.add_timer(socket.fd, TIMEOUT_TIME) || _eni.add_event(socket.fd, EVFILT_READ) ||
         _eni.add_event(socket.fd, EVFILT_WRITE) || _eni.disable_event(socket.fd, EVFILT_WRITE));

    if (error) {
        ::close(accept_fd);
        throw std::runtime_error("eni: " + std::string(strerror(errno)));
    }

    if (_used_connections >= _max_connections) {
        for (std::vector<Connection>::iterator it = _v_connection.begin();
             it != _v_connection.end(); ++it) {
            if (!it->is_active()) {
                close_connection(it);
                it->init(accept_fd, client_addr, socket.addr);
                return;
            }
        }
        ::close(accept_fd);
    } else {
        std::vector<Connection>::iterator it =
            std::find(_v_connection.begin(), _v_connection.end(), -1);
        it->init(accept_fd, client_addr, socket.addr);
        _used_connections++;
    }
}

void Webserver::receive(int fd, ssize_t data_len) {
    ssize_t to_recv_len = data_len < _read_buf_size ? data_len : _read_buf_size;
    ssize_t recved_len = ::recv(fd, _read_buf, to_recv_len, 0);
    if (recved_len != to_recv_len) {
        close_connection(fd);
        throw std::runtime_error("recv: failed");
    }

    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);

    it->parse_request(_read_buf, recved_len);
    if (it.is_request_done()) {
        int error = (_eni.disable_event(fd, EVFILT_READ) || _eni.enable_event(fd, EVFILT_WRITE));
        if (error) {
            close_connection(it);
            throw std::runtime_error("eni: " + std::string(strerror(errno)));
        }
    }
}

void Webserver::send(int fd, ssize_t max_len) {
    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);
    it.send_response(max_len);
}

void Webserver::close_connection(int fd) {
    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);

    close(it);
}

void Webserver::close_connection(std::vector<Connection>::iterator it) {
    _eni.delete_event(it.fd(), TIMEOUT_TIME);
    _eni.delete_event(it.fd(), EVFILT_READ);
    _eni.delete_event(it.fd(), EVFILT_WRITE);
    it.destroy();
}

void Webserver::timeout_connection(int fd) {
    std::cerr << "Timeout: ";
    close_connection(fd);
}

}  // namespace core
