#include "StatusCode.hpp"

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
        core::ByteBuffer buf(0);
        buf.append("<html>\n<head><title>", 20);
        buf.append(static_status_codes[i].msg, strlen(static_status_codes[i].msg));
        buf.append("</title></head>\n<body>\n<center><h1>", 35);
        buf.append(static_status_codes[i].msg, strlen(static_status_codes[i].msg));
        buf.append("</h1></center>\n<hr><center>webserv</center>\n</body>\n</html>\n", 60);
        codes.insert(std::make_pair(static_status_codes[i].code, buf));
    }
}

}  // namespace http
