#pragma once

#include "../config/Location.hpp"
#include "../config/Server.hpp"
#include "../core/ByteBuffer.hpp"
#include "Request.hpp"

namespace http {

class Response {
   private:
    core::ByteBuffer& _buf;

   public:
    CONNECTION_STATE connection_state;

    Response(core::ByteBuffer& buf);
    ~Response();

    void init(const Request& request, const config::Server& server,
              const config::Location& location);
};

}  // namespace http
