#pragma once

#include "Request.hpp"
#include "Response.hpp"

namespace http {

class RequestHandler {
   private:
   public:
    http::Response* get_response(const http::Request& req);
};

}  // namespace http
