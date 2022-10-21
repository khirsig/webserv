#pragma once

#include "../config/Location.hpp"
#include "../config/Server.hpp"
#include "../core/ByteBuffer.hpp"
#include "../file/Handler.hpp"
#include "Request.hpp"

namespace http {

enum ResponseSate { RESPONSE_HEADER, RESPONSE_BODY, RESPONSE_DONE };
enum ResponseContent { RESPONSE_CONTENT_NONE, RESPONSE_CONTENT_FILE, RESPONSE_CONTENT_CGI };

class Response {
   private:
    // core::ByteBuffer& _buf;

    config::Redirect* _find_redir_file(config::Location* location, const std::string& file_path);
    config::Redirect* _find_redir_folder(config::Location* location);
    void              _respond_redir(const config::Redirect& redir);
    void              _construct_header();

   public:
    CONNECTION_STATE connection_state;
    file::Handler    file;
    ResponseSate     state;
    ResponseContent  content;
    core::ByteBuffer buf;

    Response();
    ~Response();

    void init(const Request& request, config::Server& server, config::Location& location);
};

}  // namespace http
