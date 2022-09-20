#include "Request.hpp"

namespace core {

inline int Request::check_version(const char *version) {
    if (strcmp(version, "HTTP/1.1")) {
        _status_code = 505;
        return -1;
    }
    return 0;
}

inline int Request::check_method(const char *method) {
    for (size_t i = 0; i < sizeof(valid_methods) / sizeof(valid_methods[0]); i++) {
        if (method == valid_methods[i].first) {
            if (valid_methods[i].second == IMPLEMENTED) {
                std::cerr << "valid method: \"" << method << "\"\n";
                _method = int(i);
                return 0;
            }
            std::cerr << "method not implemented: \"" << method << "\"\n";
            _status_code = 501;
            return -1;
        }
    }
    std::cerr << "invalid method: \"" << method << "\"\n";
    _status_code = 400;
    return -1;
}

inline int Request::check_uri(const char *uri) {
    if (uri[0] != '/' && strcmp(uri, "http://")) {
        _status_code = 401;
        return -1;
    }
    for (size_t i = 0; uri[i] != '\0'; i++) {
        if (!isprint(uri[i])) {
            _status_code = 402;
            return -1;
        } else if (uri[i] == '%' && (!isxdigit(uri[i + 1]) || !isxdigit(uri[i + 2]))) {
            _status_code = 403;
            return -1;
        }
    }
    _uri = uri;
    return 0;
}

void skip_whitespaces(const char *str, const size_t &len, size_t &i) {
    while (i < len && str[i] == ' ') {
        i++;
    }
}

char *get_next_str(char *req_line, const size_t &len, size_t &i) {
    if (i > 0)
        skip_whitespaces(req_line, len, i);
    char *next_str = &(req_line[i]);
    while (i < len && req_line[i] != ' ') {
        i++;
    }
    req_line[i] = '\0';
    if (i < len)
        i++;
    return next_str;
}

int Request::parse_request_line(char *req_line, const size_t &len) {
    size_t i = 0;

    char *method = get_next_str(req_line, len, i);
    if (check_method(method))
        return _status_code;

    char *uri = get_next_str(req_line, len, i);
    std::cerr << "URI: \"" << uri << "\"\n";
    if (check_uri(uri))
        return _status_code;

    char *version = get_next_str(req_line, len, i);
    std::cerr << "Version: \"" << version << "\"\n";
    if (check_version(version))
        return _status_code;

    skip_whitespaces(req_line, len, i);
    if (i != len)
        _status_code = 400;
    return _status_code;
}

}  // namespace core