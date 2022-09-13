#include "Connections.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>

Connections::Connections(size_t max_connections) : _max_connections(max_connections) {
    _v_fd.resize(max_connections, -1);
    _v_address.resize(max_connections);
    _v_address_len.resize(max_connections, sizeof(_v_address[0]));
}

Connections::~Connections() {
    for (size_t i = 0; i < _v_fd.size(); i++) {
        if (_v_fd[i] != -1) {
            close(_v_fd[i]);
        }
    }
}

int Connections::accept_connection(int fd, Eni& kq) {
    int index = get_index(-1);
    if (index == -1) {
        struct sockaddr addr;
        socklen_t       addr_len;
        close(accept(fd, &addr, &addr_len));
        std::cerr << "refused connection, limit of max connections reached\n";
        return -1;
    }

    _v_fd[index] = accept(fd, (struct sockaddr*)&(_v_address[index]), &(_v_address_len[index]));
    if (_v_fd[index] == -1) {
        std::cerr << "accept: " << strerror(errno) << '\n';
        return -1;
    }

    kq.add_event(_v_fd[index], EVFILT_READ);

    return _v_fd[index];
}

int Connections::close_connection(int fd, Eni& kq) {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    _v_fd[index] = -1;
    kq.delete_event(fd, EVFILT_READ);
    return close(fd);
}

std::string Connections::get_connection_ip(int fd) const {
    int index = get_index(fd);
    if (index == -1)
        return std::string();
    return std::string(inet_ntoa(_v_address[index].sin_addr));
}

int Connections::get_connection_port(int fd) const {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    return (int)ntohs(_v_address[index].sin_port);
}

int Connections::get_index(int fd) const {
    for (size_t i = 0; i < _max_connections; i++) {
        if (_v_fd[i] == fd) {
            return int(i);
        }
    }
    return -1;
}
