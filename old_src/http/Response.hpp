#pragma once

#include "../cgi/Executor.hpp"
#include "../config/Location.hpp"
#include "../config/Server.hpp"
#include "../core/ByteBuffer.hpp"
#include "../core/EventNotificationInterface.hpp"
#include "../file/Handler.hpp"
#include "Request.hpp"

namespace cgi {
class Executor;
}

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
    void              _construct_header_cgi();

   public:
    CONNECTION_STATE  connection_state;
    file::FileHandler file;
    ResponseSate      state;
    ResponseContent   content;
    bool              cgi_done;
    core::ByteBuffer  header;
    core::ByteBuffer  buf;
    cgi::Executor*    executor;

    Response();
    ~Response();

    void init(Request& request, config::Location& location, core::EventNotificationInterface& eni);
};

}  // namespace http
