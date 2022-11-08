#pragma once

#include <sys/event.h>
#include <sys/socket.h>

#include "Address.hpp"

namespace core {

class Socket {
   private:
    int     _fd;
    Address _addr;

    void _socket(int family, int type, int protocol);
    void _setsockopt(int level, int option_name, int option_value);
    void _bind(in_addr_t bind_addr, in_port_t port);
    void _listen(int backlog = SOMAXCONN);

   public:
    Socket(in_addr_t bind_addr, in_port_t port);
    ~Socket();

    int            fd() const;
    const Address &addr() const;

    int close();
};

}  // namespace core
