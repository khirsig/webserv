#include "Socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>

#include <cerrno>
#include <stdexcept>

Socket::Socket(in_addr_t bind_addr, in_port_t port) {
    socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl: " + std::string(strerror(errno)));
    bind(bind_addr, port);
    listen(SOMAXCONN);
}

Socket::Socket(const Socket& other) : fd(other.fd) {}

Socket::~Socket() {}

Socket& Socket::operator=(const Socket& other) {
    if (this != &other) {
        fd = other.fd;
    }
    return *this;
}

int Socket::socket(int family, int type, int protocol) {
    fd = ::socket(family, type, protocol);
    if (fd == 0) {
        throw std::runtime_error("socket: " + std::string(strerror(errno)));
        return -1;
    }
    return fd;
}

int Socket::setsockopt(int level, int option_name, int option_value) {
    if (::setsockopt(fd, level, option_name, &option_value, sizeof(option_value))) {
        throw std::runtime_error("setsockopt: " + std::string(strerror(errno)));
        return -1;
    }
    return 0;
}

int Socket::bind(in_addr_t bind_addr, in_port_t port) {
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = bind_addr;
    address.sin_port = htons(port);
    if (::bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("bind: " + std::string(strerror(errno)));
        return -1;
    }
    return 0;
}

int Socket::listen(int backlog) {
    if (::listen(fd, backlog) < 0) {
        throw std::runtime_error("listen: " + std::string(strerror(errno)));
        return -1;
    }
    return 0;
}

int Socket::close() { return ::close(fd); }

bool operator==(const Socket& lhs, const Socket& rhs) { return lhs.fd == rhs.fd; }
bool operator==(const Socket& lhs, int rhs) { return lhs.fd == rhs; }
bool operator==(int lhs, const Socket& rhs) { return lhs == rhs.fd; }
