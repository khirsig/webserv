#include "Response.hpp"

namespace http {

Response::Response(core::ByteBuffer& buf) : _buf(buf) {}

Response::~Response() {}

void Response::init(const Request& request, const config::Server& server,
                    const config::Location& location) {
    connection_state = request.connection_state;
    _buf.append("HTTP/1.1 200 OK\nContent-Length: 9\nContent-Type: text/plain\n\nRESPONSE\n");
    // check redir
    //  if file
    //      check for file redir
    //  if folder
    //      check for redir .
    //
    // redir target starts with / then redir "Location: http://host:port/target"
    // else redir "Location: target"

    // if path ends with / dann dir sonst file
    //      - check for index files
    //      if no index file
    //          - check for dir listing
    //              if no dir listing
    //                  error 404
    //              else
    //                  dir_listing
    // else if file
    //      - file exists
    //      if no file
    //          error 404
    //      else
    //          return file content
    //

    // if (cgi) -> go to cgi stuff
    // if (not cgi) only method GET allowed
    // open file with "root + request.path"
    // Construct response headers with chunked oder content-len based on file_size
}

}  // namespace http
