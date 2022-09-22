#pragma once

#include <netinet/in.h>

#include <vector>

#include "../http/Request.hpp"
#include "ByteBuffer.hpp"
#include "EventNotificationInterface.hpp"

#define CONNECTION_TIMEOUT 600000

namespace core {

class Connections {
   public:
    Connections(size_t max_connections);
    ~Connections();

    int         accept_connection(int fd, EventNotificationInterface& kq);
    int         close_connection(int fd, EventNotificationInterface& kq);
    std::string get_connection_ip(int fd) const;
    int         get_connection_port(int fd) const;
    int         receive(int fd);

   private:
    std::vector<int>                  _v_fd;
    std::vector<int>                  _v_socket_fd;
    std::vector<struct ::sockaddr_in> _v_address;
    std::vector<socklen_t>            _v_address_len;
    std::size_t                       _max_connections;
    std::vector<Request*>             _v_requests;

    int get_index(int fd) const;
};

}  // namespace core