#include "StatusCode.hpp"

#include <string>

#include "httpStatusCodes.hpp"

namespace http {

struct s_status_codes {
    int         code;
    const char *msg;
};

static const struct s_status_codes static_status_codes[] = {
    {HTTP_OK, HTTP_OK_MSG},
    {HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG},
    {HTTP_BAD_REQUEST, HTTP_BAD_REQUEST_MSG},
    {HTTP_FORBIDDEN, HTTP_FORBIDDEN_MSG},
    {HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG},
    {HTTP_METHOD_NOT_ALLOWED, HTTP_METHOD_NOT_ALLOWED_MSG},
    {HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG},
    {HTTP_NOT_IMPLEMENTED, HTTP_NOT_IMPLEMENTED_MSG},
    {HTTP_VERSION_NOT_SUPPORTED, HTTP_VERSION_NOT_SUPPORTED_MSG}};

void StatusCode::init() {
    for (size_t i = 0; i < sizeof(static_status_codes) / sizeof(static_status_codes[0]); i++) {
        core::ByteBuffer header(0);
        core::ByteBuffer body(0);

        // BODY
        body.append("<html>\r\n<head><title>");
        body.append(static_status_codes[i].msg);
        body.append("</title></head>\r\n<body>\r\n<center><h1>");
        body.append(static_status_codes[i].msg);
        body.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");

        // HEADER
        header.append("HTTP/1.1 ");
        header.append(static_status_codes[i].msg);
        header.append("\r\nServer: webserv\r\n");
        header.append("Content-Type: text/html\r\n");
        header.append("Content-Length: ");
        header.append(std::to_string(body.size()).c_str());
        header.append("\r\nConnection: close\r\n\r\n");

        codes.insert(std::make_pair(static_status_codes[i].code, header + body));
    }
}

}  // namespace http
