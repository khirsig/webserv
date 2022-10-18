#pragma once

#include "../core/ByteBuffer.hpp"

namespace http {

class Response {
   private:
    core::ByteBuffer& _buf;

   public:
    Response(core::ByteBuffer& buf);
    ~Response();
};

}  // namespace http
