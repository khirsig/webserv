#pragma once

#include <map>

#include "../core/ByteBuffer.hpp"
#include "../core/FileHandler.hpp"
#include "Request.hpp"
#include "error_page.hpp"

namespace http {

class Response {
   public:
    enum BodyType { BODY_NONE, BODY_CGI, BODY_FILE, BODY_BUFFER };
    enum State { HEADER, HEADER_CGI, BODY, DONE };

   private:
    core::FileHandler      _file_handler;
    BodyType               _body_type;
    State                  _state;
    core::ByteBuffer       _header;
    core::ByteBuffer       _body;
    const config::CgiPass *_cgi_pass;

    static const std::map<int, error_page_t> _m_error_page;

    void _construct_header_file(const Request &req);
    void _construct_header_cgi(const Request &req);

    const config::Redirect *_find_redir(const config::Location *location,
                                        const std::string &relative_path, bool dir);
    bool _find_index(const config::Location *location, const std::string &absolute_path);
    const config::CgiPass *_find_cgi_pass(const config::Location *location,
                                          const std::string      &path);
    void                   _build_redir(const Request &req, const config::Redirect &redir);

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

    bool                   need_cgi() const;
    const config::CgiPass *cgi_pass() const;
};

}  // namespace http
