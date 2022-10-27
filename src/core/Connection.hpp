#pragma once

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "Address.hpp"
#include "CgiHandler.hpp"

namespace core {

class Connection {
   private:
    http::Request  _request;
    http::Response _response;
    Address        _socket_addr;
    Address        _client_addr;
    bool           _should_close;
    bool           _is_active;
    CgiHandler     _cgi_handler;

   public:
};

}  // namespace core