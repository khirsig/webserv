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
    Response();
    ~Response();

    State             state() const;
    BodyType          body_type() const;
    core::ByteBuffer &header();
    core::ByteBuffer &body();

    void               set_state(State new_state);
    core::FileHandler &file_handler();

    // void build_error(int error_code);
};

}  // namespace http
