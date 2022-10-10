#pragma once

#include "../core/ByteBuffer.hpp"

namespace http {

class Response {
   private:
   public:
    core::ByteBuffer buf;
    std::size_t      write_pos;
};

}  // namespace http
