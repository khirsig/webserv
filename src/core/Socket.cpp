#include "Socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>

#include <cerrno>
#include <stdexcept>

namespace core {

Socket::Socket(in_addr_t bind_addr, in_port_t port) {
    _socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    _bind(bind_addr, port);
    _listen(SOMAXCONN);

    struct sockaddr_in addr;
    socklen_t          addr_len = sizeof(struct sockaddr_in);
    if (getsockname(_fd, (struct sockaddr *)&addr, &addr_len) == -1)
        throw std::runtime_error("getsockname: " + std::string(strerror(errno)));
    _addr.addr = addr.sin_addr.s_addr;
    _addr.port = addr.sin_port;
}

Socket::~Socket() {}

int Socket::fd() const { return _fd; }

const Address &Socket::addr() const { return _addr; }

int Socket::close() { return ::close(_fd); }

void Socket::_socket(int family, int type, int protocol) {
    _fd = ::socket(family, type, protocol);
    if (_fd < 0) {
        throw std::runtime_error("socket: " + std::string(strerror(errno)));
    }
}

void Socket::_setsockopt(int level, int option_name, int option_value) {
    if (::setsockopt(_fd, level, option_name, &option_value, sizeof(option_value))) {
        throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
    }
}

void Socket::_bind(in_addr_t bind_addr, in_port_t port) {
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = bind_addr;
    address.sin_port = port;
    if (::bind(_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw std::runtime_error("bind: " + std::string(strerror(errno)));
    }
}

void Socket::_listen(int backlog) {
    if (::listen(_fd, backlog) < 0) {
        throw std::runtime_error("listen: " + std::string(strerror(errno)));
    }
}

}  // namespace core
