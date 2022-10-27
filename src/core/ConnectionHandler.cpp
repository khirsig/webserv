#include "ConnectionHandler.hpp"

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

void ConnectionHandler::accept(int fd) {
    struct ::sockaddr_in accept_addr;
    socklen_t            accept_addr_len;

    int accept_fd = ::accept(fd, (struct sockaddr *)&accept_addr, &accept_addr_len);
    if (accept_fd == -1) {
        throw std::runtime_error("accept: " + std::string(strerror(errno)));
    }
}

void ConnectionHandler::receive(int fd) {}

void ConnectionHandler::send(int fd) {}

void ConnectionHandler::close(int fd) {}

void ConnectionHandler::timeout(int fd) {}

}  // namespace core
