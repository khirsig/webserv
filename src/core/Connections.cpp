#include "Connections.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>

namespace core {

Connections::Connections(size_t max_connections) : _max_connections(max_connections) {
    _v_fd.resize(max_connections, -1);
    _v_socket_fd.resize(max_connections);
    _v_address.resize(max_connections);
    _v_address_len.resize(max_connections, sizeof(_v_address[0]));
    _v_request.resize(max_connections, NULL);
    _v_request_buf.resize(max_connections, NULL);
}

Connections::~Connections() {
    for (size_t i = 0; i < _v_fd.size(); i++) {
        if (_v_fd[i] != -1) {
            close(_v_fd[i]);
        }
    }
}

int Connections::accept_connection(int fd, EventNotificationInterface& eni) {
    struct ::sockaddr_in accept_addr;
    socklen_t            accept_addr_len;

    // Accept Connection from socket (fd)
    int accept_fd = accept(fd, (struct sockaddr*)&accept_addr, &accept_addr_len);
    if (accept_fd == -1) {
        std::cerr << "accept: " << strerror(errno) << '\n';
        return -1;
    }

    // Check if connection already established
    int index = get_index(accept_fd);
    if (index != -1) {
        std::cerr << "already established connection detected\n";
        return 0;
    }

    // Check if capacity for new connection is left
    index = get_index(-1);
    if (index == -1) {
        struct sockaddr addr;
        socklen_t       addr_len;
        close(accept(fd, &addr, &addr_len));
        std::cerr << "refused connection, limit of max connections reached\n";
        return -1;
    }

    // Store new connection
    _v_fd[index] = accept_fd;
    _v_address[index] = accept_addr;
    _v_address_len[index] = accept_addr_len;

    // Set new connection fd to non-blocking
    if (fcntl(_v_fd[index], F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "fcntl: " << strerror(errno) << '\n';
        close_connection(_v_fd[index], eni);
        return -1;
    }

    // Add events for connection fd
    eni.add_event(_v_fd[index], EVFILT_READ, 0);
    eni.add_event(_v_fd[index], EVFILT_TIMER, CONNECTION_TIMEOUT);

    // Store connection relevant informations
    _v_socket_fd[index] = fd;
    _v_request_buf[index] = new core::ByteBuffer;
    _v_request[index] = new http::Request(*_v_request_buf[index]);

    std::cout << "Accepted new connection: " << get_connection_ip(_v_fd[index]) << ":"
              << get_connection_port(_v_fd[index]) << std::endl;

    return _v_fd[index];
}

int Connections::close_connection(int fd, EventNotificationInterface& eni) {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    std::cerr << "Closed connection: " << get_connection_ip(fd) << ":" << get_connection_port(fd)
              << '\n';
    eni.delete_event(fd, EVFILT_READ);
    eni.delete_event(fd, EVFILT_TIMER);
    delete _v_request_buf[index];
    _v_request_buf[index] = NULL;
    delete _v_request[index];
    _v_request[index] = NULL;
    _v_fd[index] = -1;
    return close(fd);
}

int Connections::timeout_connection(int fd, EventNotificationInterface& eni) {
    std::cerr << "Timeout on connection: " << get_connection_ip(fd) << ":"
              << get_connection_port(fd) << '\n';
    return close_connection(fd, eni);
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

int Connections::receive(int fd, EventNotificationInterface& eni) {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf), 0);
    if (bytes_read == -1)
        return -1;
    eni.add_event(fd, EVFILT_TIMER, CONNECTION_TIMEOUT);
    _v_request_buf[index]->append(buf, bytes_read);

    // REQUEST
    while (_v_request_buf[index]->pos < _v_request_buf[index]->size()) {
        try {
            _v_request[index]->parse_input();
            if (_v_request[index]-> done()) {
                write(fd, "HTTP/1.1 200 OK\nContent-Length: 8\n\nresponse",
                      strlen("HTTP/1.1 200 OK\nContent-Length: 8\n\nresponse"));
                _v_request_buf[index]->erase(
                    _v_request_buf[index]->begin(),
                    _v_request_buf[index]->begin() + _v_request_buf[index]->pos);
                _v_request_buf[index]->pos = 0;
                delete _v_request[index];
                _v_request[index] = new http::Request(*_v_request_buf[index]);
            }
        } catch (int error) {
            write(fd, "Request invalid\n", 16);
            close_connection(_v_fd[index], eni);
            return -1;
        }
    }
    return 0;
}

int Connections::get_index(int fd) const {
    for (size_t i = 0; i < _max_connections; i++) {
        if (_v_fd[i] == fd) {
            return int(i);
        }
    }
    return -1;
}

}  // namespace core
