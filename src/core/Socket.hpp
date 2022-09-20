#pragma once

#include <sys/socket.h>

#include "Connections.hpp"

#ifdef LINUX
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

namespace core {

class Socket {
   public:
    int fd;

    Socket(in_addr_t bind_addr, in_port_t port);
    Socket(const Socket& other);
    ~Socket();

    Socket& operator=(const Socket& other);

    int close();

   private:
    void _socket(int family, int type, int protocol);
    void _setsockopt(int level, int option_name, int option_value);
    void _bind(in_addr_t bind_addr, in_port_t port);
    void _listen(int backlog = SOMAXCONN);
};

bool operator==(const Socket& lhs, const Socket& rhs);
bool operator==(const Socket& lhs, int rhs);
bool operator==(int lhs, const Socket& rhs);

}