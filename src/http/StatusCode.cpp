#include "StatusCode.hpp"

#include "httpStatusCodes.hpp"

namespace http {

struct s_status_codes {
    int         code;
    const char *msg;
};

static const struct s_status_codes static_status_codes[] = {
    {HTTP_OK, HTTP_OK_MSG},
    {HTTP_MOVED_PERMANENTLY, HTTP_OK_MSG},
    {HTTP_BAD_REQUEST, HTTP_OK_MSG},
    {HTTP_FORBIDDEN, HTTP_OK_MSG},
    {HTTP_NOT_FOUND, HTTP_OK_MSG},
    {HTTP_METHOD_NOT_ALLOWED, HTTP_OK_MSG},
    {HTTP_INTERNAL_SERVER_ERROR, HTTP_OK_MSG},
    {HTTP_NOT_IMPLEMENTED, HTTP_OK_MSG},
    {HTTP_VERSION_NOT_SUPPORTED, HTTP_OK_MSG}};

void StatusCode::init() {
    for (size_t i = 0; i < sizeof(static_status_codes) / sizeof(static_status_codes[0]); i++) {
        core::ByteBuffer header(0);
        core::ByteBuffer body(0);

        // HEADER
        header.append("HTTP/1.1 ", 9);
        header.append("\nServer: webserv\n", 17);
        header.append("Content-Type: text/html\n", 24);

        // HEADER
        body.append("<html>\n<head><title>", 20);
        body.append(static_status_codes[i].msg, strlen(static_status_codes[i].msg));
        body.append("</title></head>\n<body>\n<center><h1>", 35);
        body.append(static_status_codes[i].msg, strlen(static_status_codes[i].msg));
        body.append("</h1></center>\n<hr><center>webserv</center>\n</body>\n</html>\n", 60);
        codes.insert(std::make_pair(static_status_codes[i].code, body));
    }
}

}  // namespace http
