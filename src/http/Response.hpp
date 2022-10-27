#pragma once

#include "../core/FileHandler.hpp"

namespace http {

class Response {
   public:
    enum BodyType { NONE, CGI, FILE, BUFFER };
    enum State { HEADER, BODY, DONE };

   private:
    core::FileHandler _file_handler;
    BodyType          _body_type;
    State             _state;
    core::ByteBuffer  _header;
    core::ByteBuffer  _body;

   public:
    State state() const;
};

}  // namespace http