#pragma once

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "Address.hpp"
#include "CgiHandler.hpp"

namespace core {

class Connection {
   private:
    int            _fd;
    http::Request  _request;
    http::Response _response;
    Address        _socket_addr;
    Address        _client_addr;
    bool           _should_close;
    bool           _is_active;
    bool           _is_request_done;
    int            _request_error;
    CgiHandler     _cgi_handler;

    static const std::string _max_pipe_size_str;

   public:
    Connection();
    ~Connection();

    bool is_active() const;
    bool is_request_done() const;

    void init(int fd, Address client_addr, Address socket_addr);
    void parse_request(char *read_buf, size_t len);
    void build_response();
    void send_response(size_t max_len);
};

}  // namespace core
