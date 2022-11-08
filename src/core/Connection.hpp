#pragma once

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../settings.hpp"
#include "Address.hpp"
#include "CgiHandler.hpp"
#include "EventNotificationInterface.hpp"

namespace core {

class Connection {
   private:
    int            _fd;
    char*          _buf;
    size_t         _buf_pos;
    size_t         _buf_filled;
    http::Request  _request;
    http::Response _response;
    Address        _socket_addr;
    Address        _client_addr;
    bool           _should_close;
    bool           _is_active;
    bool           _is_request_done;
    bool           _is_response_done;
    int            _request_error;
    CgiHandler     _cgi_handler;

    const size_t             BUF_SIZE;
    static const std::string _max_pipe_size_str;

   public:
    Connection();
    ~Connection();

    bool is_active() const;
    bool is_request_done() const;
    bool is_response_done() const;
    bool should_close() const;

    int fd() const;

    void init(int fd, Address client_addr, Address socket_addr);
    void reinit();
    void receive(size_t data_len);
    void parse_request(const std::vector<config::Server>& v_server);
    void build_response(EventNotificationInterface& eni);
    void send_response(EventNotificationInterface& eni, size_t max_len);
    void destroy();
};

bool operator==(const Connection& lhs, int fd);
bool operator==(int fd, const Connection& rhs);
bool operator==(const Connection& lhs, const Connection& rhs);

}  // namespace core
