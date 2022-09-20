#pragma once

#include <netinet/in.h>

#include <vector>

#include "EventNotificationInterface.hpp"
#include "Request.hpp"

#define BUFFER_SIZE 2048

#define CONNECTION_TIMEOUT 60000

namespace core {

class Connections {
   public:
    Connections(size_t max_connections);
    ~Connections();

    int         accept_connection(int fd, EventNotificationInterface& kq);
    int         close_connection(int fd, EventNotificationInterface& kq);
    std::string get_connection_ip(int fd) const;
    int         get_connection_port(int fd) const;

   private:
    std::vector<int>                  _v_fd;
    std::vector<int>                  _v_socket_fd;
    std::vector<struct ::sockaddr_in> _v_address;
    std::vector<socklen_t>            _v_address_len;
    std::size_t                       _max_connections;
    Request                           request;

    int get_index(int fd) const;
};

}  // namespace core