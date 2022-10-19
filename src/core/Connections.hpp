#pragma once

#include <netinet/in.h>

#include <vector>

#include "../config/Server.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "ByteBuffer.hpp"
#include "EventNotificationInterface.hpp"

#define CONNECTION_TIMEOUT 60000

namespace core {

class Connections {
   public:
    Connections(size_t max_connections, std::vector<config::Server>& v_server);
    ~Connections();

    void        accept_connection(int fd, EventNotificationInterface& eni);
    int         recv_request(int fd, EventNotificationInterface& eni);
    void        parse_request(int index, EventNotificationInterface& eni);
    void        build_response(int index);
    void        send_response(int fd, EventNotificationInterface& eni, size_t max_bytes);
    void        close_connection(int fd, EventNotificationInterface& eni);
    void        timeout_connection(int fd, EventNotificationInterface& eni);
    std::string get_connection_ip(int fd) const;
    int         get_connection_port(int fd) const;

   private:
    std::size_t                       _max_connections;
    std::vector<int>                  _v_fd;
    std::vector<int>                  _v_socket_fd;
    std::vector<struct ::sockaddr_in> _v_address;
    std::vector<socklen_t>            _v_address_len;
    std::vector<http::Request*>       _v_request;
    std::vector<core::ByteBuffer*>    _v_request_buf;
    std::vector<http::Response*>      _v_response;
    std::vector<core::ByteBuffer*>    _v_response_buf;
    std::vector<config::Server>&      _v_server;
    // http::RequestHandler              _r_handler;
    // std::vector<http::Response*>      _v_response;

    int               get_index(int fd) const;
    config::Server&   _find_server(int index, const http::Request& request);
    config::Location& _find_location(const http::Request& request, const config::Server& server);
};

}  // namespace core
