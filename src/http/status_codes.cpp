#include "status_codes.hpp"

#include <map>
#include <string>

namespace http {

struct s_status_code {
    int         code;
    const char *msg;
};

static const struct s_status_code static_status_codes[] = {
    {HTTP_OK, HTTP_OK_MSG},
    {HTTP_MOVED_PERMANENTLY, HTTP_MOVED_PERMANENTLY_MSG},
    {HTTP_FOUND, HTTP_FOUND_MSG},
    {HTTP_TEMPORARY_REDIRECT, HTTP_TEMPORARY_REDIRECT_MSG},
    {HTTP_PERMANENT_REDIRECT, HTTP_PERMANENT_REDIRECT_MSG},
    {HTTP_BAD_REQUEST, HTTP_BAD_REQUEST_MSG},
    {HTTP_FORBIDDEN, HTTP_FORBIDDEN_MSG},
    {HTTP_NOT_FOUND, HTTP_NOT_FOUND_MSG},
    {HTTP_METHOD_NOT_ALLOWED, HTTP_METHOD_NOT_ALLOWED_MSG},
    {HTTP_CONTENT_TOO_LARGE, HTTP_CONTENT_TOO_LARGE_MSG},
    {HTTP_INTERNAL_SERVER_ERROR, HTTP_INTERNAL_SERVER_ERROR_MSG},
    {HTTP_NOT_IMPLEMENTED, HTTP_NOT_IMPLEMENTED_MSG},
    {HTTP_VERSION_NOT_SUPPORTED, HTTP_VERSION_NOT_SUPPORTED_MSG}};

std::map<int, std::string> new_m_status_codes() {
    std::map<int, std::string> m_implemented;
    for (size_t i = 0; i < sizeof(static_status_codes) / sizeof(static_status_codes[0]); i++) {
        m_implemented[static_status_codes[i].code] = static_status_codes[i].msg;
    }
    return m_implemented;
}

}  // namespace http
