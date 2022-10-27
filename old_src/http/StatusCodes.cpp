#include "StatusCodes.hpp"

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
    {HTTP_FOUND, HTTP_FOUND_MSG},
    {HTTP_TEMPORARY_REDIRECT, HTTP_TEMPORARY_REDIRECT_MSG},
    {HTTP_PERMANENT_REDIRECT, HTTP_PERMANENT_REDIRECT_MSG},
    {HTTP_BAD_REQUEST, HTTP_BAD_REQUEST_MSG},
    {HTTP_FORBIDDEN, HTTP_FORBIDDEN_MSG},
    {HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG},
    {HTTP_METHOD_NOT_ALLOWED, HTTP_METHOD_NOT_ALLOWED_MSG},
    {HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG},
    {HTTP_NOT_IMPLEMENTED, HTTP_NOT_IMPLEMENTED_MSG},
    {HTTP_VERSION_NOT_SUPPORTED, HTTP_VERSION_NOT_SUPPORTED_MSG}};

void StatusCodes::init() {
    for (size_t i = 0; i < sizeof(static_status_codes) / sizeof(static_status_codes[0]); i++) {
        core::ByteBuffer msg(0);
        msg.append(static_status_codes[i].msg);
        codes.insert(std::make_pair(static_status_codes[i].code, msg));
    }
}

}  // namespace http
