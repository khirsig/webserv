#include "ErrorPages.hpp"

#include "StatusCodes.hpp"
#include "httpStatusCodes.hpp"

namespace http {

static const int static_error_codes[] = {
    HTTP_MOVED_PERMANENTLY, HTTP_BAD_REQUEST,           HTTP_FORBIDDEN,
    HTTP_NOT_FOUND,         HTTP_METHOD_NOT_ALLOWED,    HTTP_INTERNAL_SERVER_ERROR,
    HTTP_NOT_IMPLEMENTED,   HTTP_VERSION_NOT_SUPPORTED,
};

void ErrorPages::init() {
    for (size_t i = 0; i < sizeof(static_error_codes) / sizeof(static_error_codes[0]); i++) {
        core::ByteBuffer header(0);
        core::ByteBuffer body(0);

        // BODY
        body.append("<html>\r\n<head><title>");
        body += g_status_codes.codes[static_error_codes[i]];
        body.append("</title></head>\r\n<body>\r\n<center><h1>");
        body += g_status_codes.codes[static_error_codes[i]];
        body.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");

        // HEADER
        header.append("HTTP/1.1 ");
        header += g_status_codes.codes[static_error_codes[i]];
        header.append("\r\nServer: webserv\r\n");
        header.append("Content-Type: text/html\r\n");
        header.append("Content-Length: ");
        header.append(std::to_string(body.size()).c_str());
        header.append("\r\nConnection: close\r\n\r\n");

        pages.insert(std::make_pair(static_error_codes[i], header + body));
    }
}

}  // namespace http
