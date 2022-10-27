#include "ConnectionHandler.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <cerrno>
#include <string>

namespace core {

ConnectionHandler::ConnectionHandler(std::vector<config::Server> v_server, size_t max_connections)
    : _used_connections(0),
      _max_connections(max_connections),
      _v_server(v_server),
      _read_buf_size(MAX_PIPE_SIZE) {
    _v_connection.resize(_max_connections);
    _read_buf = new char[_read_buf_size];
}

ConnectionHandler::~ConnectionHandler() { delete[] _read_buf; }

void ConnectionHandler::accept(const Socket &socket) {
    struct ::sockaddr_in accept_addr;
    socklen_t            accept_addr_len;

    int accept_fd = ::accept(fd, (struct sockaddr *)&accept_addr, &accept_addr_len);
    if (accept_fd == -1) {
        throw std::runtime_error("accept: " + std::string(strerror(errno)));
    }

    if (fcntl(accept_fd, F_SETFL, O_NONBLOCK) == -1) {
        ::close(accept_fd);
        throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    }

    if (_used_connections >= _max_connections) {
        for (std::vector<Connection>::iterator it = _v_connection.begin();
             it != _v_connection.end(); ++it) {
            if (!it.is_active()) {
                close(it);
                // it.init(_eni, accept_addr, socket.addr);
                return;
            }
        }
        ::close(accept_fd);
    } else {
        std::vector<Connection>::iterator it =
            std::find(_v_connection.begin(), _v_connection.end(), -1);
        // it.init(_eni, accept_addr, socket.addr);
        _used_connections++;
    }
}

void ConnectionHandler::receive(int fd, ssize_t data_len) {
    ssize_t to_recv_len = data_len < _read_buf_size ? data_len : _read_buf_size;
    ssize_t recved_len = ::recv(fd, _read_buf, to_recv_len, 0);
    if (recved_len != to_recv_len) {
        close(fd);
        throw std::runtime_error("recv: failed");
    }

    std::vector<Connection>::iterator it =
        std::find(_v_connection.begin(), _v_connection.end(), fd);
    // if (it == _v_connection.end())
    // return;

    try {
        it.parse_buf(_read_buf, recved_len);
        if (it.is_done()) {
            it.build_response();
        }
    } catch (int e) {
    }
}

void ConnectionHandler::send(int fd) {}

void ConnectionHandler::close(int fd) {}

void ConnectionHandler::close(std::vector<Connection>::iterator it) {}

void ConnectionHandler::timeout(int fd) {}

}  // namespace core