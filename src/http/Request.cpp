#include "Request.hpp"

#include "StatusCodes.hpp"

#define IS_METHOD_CHAR(c) (c >= 'A' && c <= 'Z')
#define IS_SEPERATOR_CHAR(c)                                                               \
    (c == '(' || c == ')' || c == '<' || c == '>' || c == '@' || c == ',' || c == ';' ||   \
     c == ':' || c == '\\' || c == '\"' || c == '/' || c == '[' || c == ']' || c == '?' || \
     c == '=' || c == '{' || c == '}' || c == ' ' || c == '\t')
#define IS_TEXT_CHAR(c) (isprint(c) || c == '\t')
#define IS_TOKEN_CHAR(c) (isprint(c) && !IS_SEPERATOR_CHAR(c))
#define IS_URI_SUBDELIM(c)                                                                \
    (c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || \
     c == '+' || c == ',' || c == ';' || c == '=')
#define IS_UNRESERVED_URI_CHAR(c) (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
#define IS_HOST_CHAR(c) (IS_UNRESERVED_URI_CHAR(c) || IS_URI_SUBDELIM(c))
#define HEX_CHAR_TO_INT(c) (isdigit(c) ? c - '0' : tolower(c) - 87)

#define MAX_METHOD_LEN 7

namespace http {

static const std::string required_header[] = {"host"};

bool Request::parse(char *read_buf, size_t len) {
    size_t pos = 0;
    if (_state == State::REQUEST_LINE) {
        if (!_parse_request_line(read_buf, len, pos))
            return;
        _analyze_request_line();
    }
    if (_state == HEADER) {
    }
    if (_state == BODY) {
    }
    if (_state == BODY_CHUNKED) {
        // if (!parse_chunked_body())
        //     return;
        _state = DONE;
    }
    if (_state == DONE) {
        // print();
    }
}

bool Request::_parse_request_line(char *read_buf, size_t len, size_t &pos) {
    char c;
    for (size_t i = pos; i < len; i++) {
        c = read_buf[i];
        switch (_state_request_line) {
            case RL_START:
                switch (c) {
                    case '\r':
                        break;
                    case '\n':
                        break;
                    default:
                        if (!IS_METHOD_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _method_str += c;
                        _state_request_line = RL_METHOD;
                        break;
                }
                break;
            case RL_METHOD:
                if (IS_METHOD_CHAR(c) && _method_str.length() < MAX_METHOD_LEN) {
                    _method_str += c;
                    break;
                } else if (c != ' ') {
                    throw HTTP_BAD_REQUEST;
                } else {
                    _state_request_line = RL_AFTER_METHOD;
                    _parse_method();
                    break;
                }
            case RL_AFTER_METHOD:
                switch (c) {
                    case ' ':
                        break;
                    case '/':
                        _path_encoded += c;
                        _state_request_line = RL_URI_PATH;
                        break;
                    case 'h':
                        _state_request_line = RL_URI_HT;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HT:
                switch (c) {
                    case 't':
                        _state_request_line = RL_URI_HTT;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HTT:
                switch (c) {
                    case 't':
                        _state_request_line = RL_URI_HTTP;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HTTP:
                switch (c) {
                    case 'p':
                        _state_request_line = RL_URI_HTTP_COLON;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HTTP_COLON:
                switch (c) {
                    case ':':
                        _state_request_line = RL_URI_HTTP_COLON_SLASH;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HTTP_COLON_SLASH:
                switch (c) {
                    case '/':
                        _state_request_line = RL_URI_HTTP_COLON_SLASH_SLASH;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HTTP_COLON_SLASH_SLASH:
                switch (c) {
                    case '/':
                        _state_request_line = RL_URI_HOST_START;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_URI_HOST_START:
                switch (c) {
                    case '%':
                        _state_request_line = RL_URI_HOST_ENCODE_1;
                        break;
                    default:
                        if (!IS_HOST_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _host_encoded += c;
                        break;
                }
                break;
            case RL_URI_HOST:
                switch (c) {
                    case ' ':
                        _state_request_line = RL_AFTER_URI;
                        break;
                    case '%':
                        _host_encoded += c;
                        _state_request_line = RL_URI_HOST_ENCODE_1;
                        break;
                    case '/':
                        _path_encoded += c;
                        _state_request_line = RL_URI_PATH;
                        break;
                    case ':':
                        _state_request_line = RL_URI_HOST_PORT;
                        break;
                    default:
                        if (!IS_HOST_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _host_encoded += c;
                        break;
                }
                break;
            case RL_URI_HOST_ENCODE_1:
                if (!isxdigit(c))
                    throw HTTP_BAD_REQUEST;
                _host_encoded += c;
                _state_request_line = RL_URI_HOST_ENCODE_2;
                break;
            case RL_URI_HOST_ENCODE_2:
                if (!isxdigit(c))
                    throw HTTP_BAD_REQUEST;
                _host_encoded += c;
                _state_request_line = RL_URI_HOST;
                break;
            case RL_URI_HOST_PORT:
                switch (c) {
                    case ' ':
                        _state_request_line = RL_AFTER_URI;
                        break;
                    case '/':
                        _path_encoded += c;
                        _state_request_line = RL_URI_PATH;
                        break;
                    default:
                        if (!isdigit(c))
                            throw HTTP_BAD_REQUEST;
                        break;
                }
                break;
            case RL_URI_PATH:
                switch (c) {
                    case ' ':
                        _state_request_line = RL_AFTER_URI;
                        break;
                    case '%':
                        _path_encoded += c;
                        _state_request_line = RL_URI_PATH_ENCODE_1;
                        break;
                    case '?':
                        _state_request_line = RL_URI_QUERY;
                        break;
                    case '#':
                        _state_request_line = RL_URI_FRAGMENT;
                        break;
                    default:
                        if (!isprint(c))
                            throw HTTP_BAD_REQUEST;
                        _path_encoded += c;
                        break;
                }
                break;
            case RL_URI_PATH_ENCODE_1:
                if (!isxdigit(c))
                    throw HTTP_BAD_REQUEST;
                _path_encoded += c;
                _state_request_line = RL_URI_PATH_ENCODE_2;
                break;
            case RL_URI_PATH_ENCODE_2:
                if (!isxdigit(c))
                    throw HTTP_BAD_REQUEST;
                _path_encoded += c;
                _state_request_line = RL_URI_PATH;
                break;
            case RL_URI_QUERY:
                switch (c) {
                    case ' ':
                        _state_request_line = RL_AFTER_URI;
                        break;
                    case '#':
                        _state_request_line = RL_URI_FRAGMENT;
                    default:
                        if (!isprint(c))
                            throw HTTP_BAD_REQUEST;
                        _query_string += c;
                        break;
                }
                break;
            case RL_URI_FRAGMENT:
                switch (c) {
                    case ' ':
                        _state_request_line = RL_AFTER_URI;
                        break;
                    default:
                        if (!isprint(c))
                            throw HTTP_BAD_REQUEST;
                        break;
                }
                break;
            case RL_AFTER_URI:
                switch (c) {
                    case ' ':
                        break;
                    case 'H':
                        _state_request_line = RL_VERSION_HT;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_VERSION_HT:
                if (c == 'T')
                    _state_request_line = RL_VERSION_HTT;
                else
                    throw HTTP_BAD_REQUEST;
                break;
            case RL_VERSION_HTT:
                if (c == 'T')
                    _state_request_line = RL_VERSION_HTTP;
                else
                    throw HTTP_BAD_REQUEST;
                break;
            case RL_VERSION_HTTP:
                if (c == 'P')
                    _state_request_line = RL_VERSION_HTTP_SLASH;
                else
                    throw HTTP_BAD_REQUEST;
                break;
            case RL_VERSION_HTTP_SLASH:
                if (c == '/')
                    _state_request_line = RL_VERSION_HTTP_SLASH_MAJOR;
                else
                    throw HTTP_BAD_REQUEST;
                break;
            case RL_VERSION_HTTP_SLASH_MAJOR:
                switch (c) {
                    case '1':
                        _state_request_line = RL_VERSION_HTTP_SLASH_MAJOR_DOT;
                        break;
                    default:
                        if (isdigit(c))
                            throw HTTP_NOT_IMPLEMENTED;
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_VERSION_HTTP_SLASH_MAJOR_DOT:
                switch (c) {
                    case '.':
                        _state_request_line = RL_VERSION_HTTP_SLASH_MAJOR_DOT_MINOR;
                        break;
                    default:
                        if (isdigit(c))
                            throw HTTP_NOT_IMPLEMENTED;
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_VERSION_HTTP_SLASH_MAJOR_DOT_MINOR:
                switch (c) {
                    case '1':
                        _state_request_line = RL_AFTER_VERSION;
                        break;
                    default:
                        if (isdigit(c))
                            throw HTTP_NOT_IMPLEMENTED;
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_AFTER_VERSION:
                switch (c) {
                    case ' ':
                        break;
                    case '\r':
                        _state_request_line = RL_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_request_line = RL_DONE;
                        break;
                    default:
                        if (isdigit(c))
                            throw HTTP_NOT_IMPLEMENTED;
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case RL_ALMOST_DONE:
                if (c == '\n')
                    _state_request_line = RL_DONE;
                else
                    throw HTTP_BAD_REQUEST;
                break;
            case RL_DONE:
                break;
        }
        if (_state_request_line == RL_DONE)
            return true;
    }
    return false;
}

}  // namespace http
