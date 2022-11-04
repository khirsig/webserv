#pragma once

#include <map>

#include "../core/ByteBuffer.hpp"
#include "../core/FileHandler.hpp"
#include "Request.hpp"
#include "error_page.hpp"

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

    static const std::map<int, error_page_t> _m_error_page;

    // void _construct_header_buffer(int status_code, const std::string &content_type);
    void _construct_header_file();
    void _construct_header_cgi();

   public:
    Response();
    ~Response();

    State             state() const;
    BodyType          body_type() const;
    core::ByteBuffer &header();
    core::ByteBuffer &body();

    void               set_state(State new_state);
    core::FileHandler &file_handler();

    void init();

    void build(const Request &req);
    void build_error(const Request &req, int error_code);
};

}  // namespace http
