#pragma once

#include "../config/Location.hpp"
#include "../config/Server.hpp"
#include "../core/ByteBuffer.hpp"
#include "Request.hpp"

namespace http {

class Response {
   private:
    core::ByteBuffer& _buf;

    config::Redirect* _find_redir_file(config::Location* location, const std::string& file_path);
    config::Redirect* _find_redir_folder(config::Location* location);
    void              _respond_redir(const config::Redirect& redir);

   public:
    CONNECTION_STATE connection_state;

    Response(core::ByteBuffer& buf);
    ~Response();

    void init(const Request& request, config::Server& server, config::Location& location);
};

}  // namespace http
