#pragma once

#include "../core/FileHandler.hpp"

namespace http {

class Response {
   public:
    enum BodyType { NONE, CGI, FILE, BUFFER };

   private:
    core::FileHandler _file_handler;
    BodyType          _body_type;
    core::ByteBuffer  _header;
    core::ByteBuffer  _body;

   public:
};

}  // namespace http