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
    _v_requests.resize(max_connections, NULL);
}

Connections::~Connections() {
    for (size_t i = 0; i < _v_fd.size(); i++) {
        if (_v_fd[i] != -1) {
            close(_v_fd[i]);
        }
    }
}

int Connections::accept_connection(int fd, EventNotificationInterface& eni) {
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

    if (fcntl(_v_fd[index], F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "fcntl: " << strerror(errno) << '\n';
        close_connection(_v_fd[index], eni);
        return -1;
    }

    eni.add_event(_v_fd[index], EVFILT_READ, 0);
    eni.add_event(_v_fd[index], EVFILT_TIMER, CONNECTION_TIMEOUT);

    _v_socket_fd[index] = fd;
    _v_requests[index] = new Request;

    return _v_fd[index];
}

int Connections::close_connection(int fd, EventNotificationInterface& eni) {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    _v_fd[index] = -1;
    eni.delete_event(fd, EVFILT_READ);
    eni.delete_event(fd, EVFILT_TIMER);
    delete _v_requests[index];
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

int Connections::receive(int fd) {
    int index = get_index(fd);
    if (index == -1)
        return -1;
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf), 0);
    // std::cerr << "bytes_read: " << bytes_read << " # \'";
    // write(2, buf, bytes_read);
    // std::cerr << "\'\n";
    if (bytes_read == -1)
        return -1;
    if (_v_requests[index]->parse_input(buf, bytes_read)) {
        // std::cerr << "Error status: " << _v_requests[index]->_status_code << '\n';
        write(fd, "Bad request\n", 12);
        delete _v_requests[index];
        _v_requests[index] = new Request;
        return -1;
    }
    _v_requests[index]->print();
    
    // std::cout << get_connection_ip(fd) << ":" << get_connection_port(fd) << " # "
    //           << _v_requests[index]->_buffer;
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