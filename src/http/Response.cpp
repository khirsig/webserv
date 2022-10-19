#include "Response.hpp"

namespace http {

Response::Response(core::ByteBuffer& buf) : _buf(buf) {}

Response::~Response() {}

void Response::init(const Request& request, const config::Server& server,
                    const config::Location& location) {
    _buf.append("RESPONSE\n");
    // check method allowed....
    // check redir
    // if (cgi) -> go to cgi stuff
    // if (not cgi) only method GET allowed
    // open file with "root + request.path"
    // Construct response headers with chunked oder content-len based on file_size
}

}  // namespace http
