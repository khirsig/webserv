#pragma once

#include <netinet/in.h>

#include <vector>

#include "../http/Request.hpp"
#include "../http/RequestHandler.hpp"
#include "../http/Response.hpp"
#include "ByteBuffer.hpp"
#include "EventNotificationInterface.hpp"

#define CONNECTION_TIMEOUT 6000

namespace core {

class Connections {
   public:
    Connections(size_t max_connections);
    ~Connections();

    int         accept_connection(int fd, EventNotificationInterface& eni);
    int         close_connection(int fd, EventNotificationInterface& eni);
    int         timeout_connection(int fd, EventNotificationInterface& eni);
    std::string get_connection_ip(int fd) const;
    int         get_connection_port(int fd) const;
    int         receive(int fd, EventNotificationInterface& eni);

   private:
    std::size_t                       _max_connections;
    std::vector<int>                  _v_fd;
    std::vector<int>                  _v_socket_fd;
    std::vector<struct ::sockaddr_in> _v_address;
    std::vector<socklen_t>            _v_address_len;
    std::vector<http::Request*>       _v_request;
    std::vector<core::ByteBuffer*>    _v_request_buf;
    // http::RequestHandler              _r_handler;
    // std::vector<http::Response*>      _v_response;

    int get_index(int fd) const;
};

}  // namespace core
