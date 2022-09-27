#include "Request.hpp"

#include <unistd.h>

#include <algorithm>

#include "../log/Log.hpp"

namespace core {

#define MAX_BUFFER_SIZE 8192

#define IS_METHOD_CHAR(c) (c >= 'A' && c <= 'Z')

#define IS_SEPERATOR_CHAR(c)                                                               \
    (c == '(' || c == ')' || c == '<' || c == '>' || c == '@' || c == ',' || c == ';' ||   \
     c == ':' || c == '\\' || c == '\"' || c == '/' || c == '[' || c == ']' || c == '?' || \
     c == '=' || c == '{' || c == '}' || c == ' ' || c == '\t')
#define IS_TEXT_CHAR(c) (isprint(c) || c == '\t')
#define IS_TOKEN_CHAR(c) (isprint(c) && !IS_SEPERATOR_CHAR(c))

// #define IS_HEADER_KEY_CHAR(c) (IS_TOKEN_CHAR(c))
// #define IS_HEADER_VALUE_CHAR(c) (IS_TEXT_CHAR(c))

static const std::pair<std::string, bool> headers[] = {std::make_pair("host", false),
                                                       std::make_pair("connection", true),
                                                       std::make_pair("content-length", true)};

Request::Request()
    : _buf_pos(0),
      _request_end(0),
      _method_start(0),
      _method_end(0),
      _method(NONE),
      _version_start(0),
      _version_end(0),
      _uri_start(0),
      _uri_end(0),
      _uri_path_start(0),
      _uri_path_end(0),
      _uri_query_start(0),
      _uri_query_end(0),
      _uri_fragment_start(0),
      _uri_fragment_end(0),
      _header_start(0),
      _header_end(0),
      _header_key_start(0),
      _header_key_end(0),
      _header_value_start(0),
      _header_value_end(0),
      _state(REQUEST_LINE),
      _state_request_line(START),
      _state_header(H_START) {
    _buf.reserve(8192);
}

Request::~Request() {}

int Request::parse_input(const char* input, std::size_t len) {
    _buf.append(input, len);
    if (_state == REQUEST_LINE) {
        int status = parse_request_line();
        if (status == 0) {
            _state = HEADER;
        } else if (status > 99) {
            std::cerr << "Error: " << status << '\n';
            return -1;
        }
    }
    if (_state == HEADER) {
        int status = parse_header();
        std::cout << "status: " << status << "\n";
        if (status == 0) {
            _state = END;
        } else if (status > 99) {
            std::cerr << "Error: " << status << '\n';
            return -1;
        }
    }
    if (_state == END)
        print();
    return 0;
}

int Request::parse_request_line() {
    char c;
    // for (ByteBuffer::iterator it = _buf.begin() + _buf_pos; it != _buf.end(){
    for (std::size_t i = _buf_pos; i < _buf.size(); _buf_pos++, i++) {
        c = _buf[i];
        switch (_state_request_line) {
            case START:
                _method_start = _buf_pos;
                if (c == '\r' || c == '\n')
                    break;
                _state_request_line = METHOD;
                // break;
            case METHOD:
                if (IS_METHOD_CHAR(c))
                    break;
                if (c != ' ')
                    return 490;
                _method_end = _buf_pos;
                switch (_method_end - _method_start) {
                    case 3:
                        if (_buf.equal(_buf.begin() + _method_start, "GET", 3)) {
                            _method = GET;
                            break;
                        }
                        if (_buf.equal(_buf.begin() + _method_start, "PUT", 3)) {
                            return 501;
                        }
                        return 491;
                    case 4:
                        if (_buf.equal(_buf.begin() + _method_start, "HEAD", 4)) {
                            _method = HEAD;
                            break;
                        }
                        if (_buf.equal(_buf.begin() + _method_start, "POST", 4)) {
                            _method = POST;
                            break;
                        }
                        return 491;
                    case 5:
                        if (_buf.equal(_buf.begin() + _method_start, "TRACE", 5)) {
                            return 501;
                        }
                        return 491;
                    case 6:
                        if (_buf.equal(_buf.begin() + _method_start, "DELETE", 6)) {
                            _method = DELETE;
                            break;
                        }
                        return 491;
                    case 7:
                        if (_buf.equal(_buf.begin() + _method_start, "CONNECT", 7)) {
                            return 501;
                        }
                        if (_buf.equal(_buf.begin() + _method_start, "OPTIONS", 7)) {
                            return 501;
                        }
                        return 491;
                    default:
                        return 492;
                }
                _state_request_line = AFTER_METHOD;
                break;
            case AFTER_METHOD:
                if (c == ' ')
                    break;
                _uri_start = _buf_pos;
                switch (c) {
                    case '/':
                        _uri_path_start = _buf_pos;
                        _state_request_line = URI_SLASH;
                        break;
                    case 'h':
                        // _state_request_line = URI_HTTP;
                        // break;
                        return 1200;
                    default:
                        return 493;
                }
                break;
            case URI_SLASH:
                switch (c) {
                    case ' ':
                        _uri_end = _buf_pos;
                        _uri_path_end = _buf_pos;
                        _state_request_line = AFTER_URI;
                        break;
                    case '\r':
                        return 400;
                    case '\n':
                        return 400;
                    // case '.':
                    //     _uri_complex = true;
                    //     break;
                    case '%':
                        _state_request_line = URI_ENCODE_1;
                        break;
                    case '?':
                        _uri_path_end = _buf_pos;
                        _uri_query_start = _buf_pos + 1;
                        _state_request_line = URI_QUERY;
                        break;
                    case '#':
                        _uri_path_end = _buf_pos;
                        _uri_fragment_start = _buf_pos + 1;
                        _state_request_line = URI_FRAGMENT;
                        break;
                    default:
                        if (!isprint(c))
                            return 400;
                        break;
                }
                break;
            case URI_ENCODE_1:
                if (!isxdigit(c))
                    return 400;
                _state_request_line = URI_ENCODE_2;
                break;
            case URI_ENCODE_2:
                if (!isxdigit(c))
                    return 400;
                _state_request_line = URI_SLASH;
                break;
            case URI_QUERY:
                switch (c) {
                    case ' ':
                        _uri_end = _buf_pos;
                        _uri_query_end = _buf_pos;
                        _state_request_line = AFTER_URI;
                        break;
                    case '\r':
                        return 400;
                    case '\n':
                        return 400;
                    case '#':
                        _uri_query_end = _buf_pos;
                        _uri_fragment_start = _buf_pos + 1;
                        _state_request_line = URI_FRAGMENT;
                    default:
                        if (!isprint(c))
                            return 400;
                        break;
                }
                break;
            case URI_FRAGMENT:
                switch (c) {
                    case ' ':
                        _uri_end = _buf_pos;
                        _uri_fragment_end = _buf_pos;
                        _state_request_line = AFTER_URI;
                        break;
                    case '\r':
                        return 400;
                    case '\n':
                        return 400;
                    default:
                        if (!isprint(c))
                            return 400;
                        break;
                }
                break;
            case URI_HTTP:
                break;
            case AFTER_URI:
                if (c == ' ')
                    break;
                _state_request_line = VERSION_H;
                _version_start = _buf_pos;
                //  break;
            case VERSION_H:
                if (c == 'H')
                    _state_request_line = VERSION_HT;
                else
                    return 4951;
                break;
            case VERSION_HT:
                if (c == 'T')
                    _state_request_line = VERSION_HTT;
                else
                    return 4952;
                break;
            case VERSION_HTT:
                if (c == 'T')
                    _state_request_line = VERSION_HTTP;
                else
                    return 4953;
                break;
            case VERSION_HTTP:
                if (c == 'P')
                    _state_request_line = VERSION_HTTP_SLASH;
                else
                    return 4954;
                break;
            case VERSION_HTTP_SLASH:
                if (c == '/')
                    _state_request_line = VERSION_HTTP_SLASH_MAJOR;
                else
                    return 4955;
                break;
            case VERSION_HTTP_SLASH_MAJOR:
                if (c == '1')
                    _state_request_line = VERSION_HTTP_SLASH_MAJOR_DOT;
                else if (isdigit(c))
                    return 501;
                else
                    return 4957;
                break;
            case VERSION_HTTP_SLASH_MAJOR_DOT:
                if (c == '.')
                    _state_request_line = VERSION_HTTP_SLASH_MAJOR_DOT_MINOR;
                else if (isdigit(c))
                    return 501;
                else
                    return 4958;
                break;
            case VERSION_HTTP_SLASH_MAJOR_DOT_MINOR:
                if (c == '1')
                    _state_request_line = AFTER_VERSION;
                else if (isdigit(c))
                    return 501;
                else
                    return 4959;
                _version_end = _buf_pos + 1;
                break;
            case AFTER_VERSION:
                if (c == ' ')
                    break;
                else if (c == '\r')
                    _state_request_line = ALMOST_DONE;
                else if (c == '\n')
                    _state_request_line = DONE;
                else
                    return 496;
                break;
            case ALMOST_DONE:
                if (c == '\n')
                    _state_request_line = DONE;
                else
                    return 497;
                break;
            case DONE:
                break;
        }
        if (_state_request_line == DONE) {
            _buf_pos++;
            _request_end = _header_start = _buf_pos;
            return 0;
        }
    }
    return 1;
}

void print_header_pair(const ByteBuffer& buf, size_t key_start, size_t key_end, size_t value_start,
                       size_t value_end) {
    std::cout << "Key: \'";
    for (; key_start != key_end; key_start++) {
        std::cout << buf[key_start];
    }
    std::cout << "\'\n";
    std::cout << "Value: \'";
    for (; value_start != value_end; value_start++) {
        std::cout << buf[value_start];
    }
    std::cout << "\'\n\n";
}

int Request::_add_header() {
    if (_header_value_start == _header_value_end)
        return 0;
    std::string key(_buf.begin() + _header_key_start, _buf.begin() + _header_key_end);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    for (size_t i = 0; i < sizeof(headers) / sizeof(headers[0]); i++) {
        if (key == headers[i].first) {
            std::string value(_buf.begin() + _header_value_start, _buf.begin() + _header_value_end);
            std::pair<std::map<std::string, std::string>::iterator, bool> ret;
            ret = m_header.insert(std::make_pair(key, value));
            if (ret.second == false) {
                if (headers[i].second == false)
                    return 489;
                else {
                    (*ret.first).second += ", ";
                    (*ret.first).second += value;
                }
            }
            break;
        }
    }
    return 0;
}

int Request::parse_header() {
    char c;
    for (std::size_t i = _buf_pos; i < _buf.size(); _buf_pos++, i++) {
        c = _buf[i];
        switch (_state_header) {
            case H_START:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_REQUEST;
                        break;
                    case '\n':
                        _state_header = H_DONE_REQUEST;
                        break;
                    default:
                        if (!IS_TOKEN_CHAR(c))
                            return 481;
                        _state_header = H_KEY;
                        _header_key_start = _buf_pos;
                        break;
                }
                break;
            case H_KEY:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_LINE;
                        break;
                    case '\n':
                        _state_header = H_DONE_LINE;
                        break;
                    case ':':
                        _state_header = H_AFTER_KEY;
                        _header_key_end = _buf_pos;
                        break;
                    default:
                        if (!IS_TOKEN_CHAR(c))
                            return 482;
                        break;
                }
                break;
            case H_AFTER_KEY:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_LINE;
                        break;
                    case '\n':
                        _state_header = H_DONE_LINE;
                        break;
                    case '\t':
                    case ' ':
                        break;
                    default:
                        if (!IS_TEXT_CHAR(c))
                            return 483;
                        _state_header = H_VALUE;
                        _header_value_start = _buf_pos;
                        break;
                }
                break;
            case H_VALUE:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_LINE;
                        _header_value_end = _buf_pos;
                        break;
                    case '\n':
                        _state_header = H_DONE_LINE;
                        _header_value_end = _buf_pos;
                        break;
                    default:
                        if (!IS_TEXT_CHAR(c))
                            return 484;
                        break;
                }
                break;
            case H_ALMOST_DONE_LINE:
                if (c != '\n')
                    return 485;
                _state_header = H_DONE_LINE;
                break;
            case H_DONE_LINE:
                _add_header();
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_REQUEST;
                        break;
                    case '\n':
                        _state_header = H_DONE_REQUEST;
                        break;
                    default:
                        if (!IS_TOKEN_CHAR(c))
                            return 486;
                        _state_header = H_KEY;
                        _header_key_start = _buf_pos;
                        break;
                }
                break;
            case H_ALMOST_DONE_REQUEST:
                if (c != '\n')
                    return 487;
                _state_header = H_DONE_REQUEST;
                break;
            case H_DONE_REQUEST:
                break;
        }
        if (_state_header == H_DONE_REQUEST) {
            _buf_pos++;
            _header_end = _buf_pos;
            return 0;
        }
    }
    return 1;
}

// int Request::parse_header() {
//     char c;
//     for (std::size_t i = _buf_pos; i < _buf.size(); _buf_pos++, i++) {
//         c = _buf[i];
//         switch (_state_header) {
//             case H_START:
//                 switch (c) {
//                     case '\r':
//                         _state_header = H_ALMOST_DONE_REQUEST;
//                         break;
//                     case '\n':
//                         _state_header = H_DONE_REQUEST;
//                         break;
//                     case ' ':
//                     case ':':
//                         return 481;
//                     default:
//                         if (!IS_TOKEN_CHAR(c))
//                             return 481;
//                         break;
//                 }
//                 _state_header = H_KEY;
//                 _header_key_start = _buf_pos;
//                 break;
//             case H_KEY:
//                 if (IS_TOKEN_CHAR(c))
//                     break;
//                 _header_key_end = _buf_pos;
//                 switch (c) {
//                     case '\r':
//                         _state_header = H_ALMOST_DONE_LINE;
//                         break;
//                     case '\n':
//                         // _add_header();
//                         _state_header = H_START;
//                         break;
//                     case ' ':
//                     case ':':
//                         _state_header = H_AFTER_KEY;
//                         break;
//                     default:
//                         return 482;
//                 }
//                 break;
//             case H_AFTER_KEY:
//                 if (c == ' ' || c == ':')
//                     break;
//                 _header_value_start = _buf_pos;
//                 _state_header = H_VALUE;
//                 // break;
//             case H_VALUE:
//                 if (IS_TEXT_CHAR(c))
//                     break;
//                 _header_value_end = _buf_pos;
//                 switch (c) {
//                     case '\r':
//                         _state_header = H_ALMOST_DONE_LINE;
//                         break;
//                     case '\n':
//                         _add_header();
//                         _state_header = H_START;
//                         // _state_header = H_DONE_LINE;
//                         break;
//                     default:
//                         return 483;
//                 }
//                 break;
//             case H_ALMOST_DONE_LINE:
//                 if (c == '\n') {
//                     // _state_header = H_DONE_LINE;
//                     _add_header();
//                     _state_header = H_START;
//                 } else
//                     return 480;
//                 break;
//             // case H_DONE_LINE:
//             //     {
//             //         _add_header();
//             //     }
//             //     _header_key_start = _header_key_end = _buf_pos;
//             //     _header_value_start = _header_value_end = 0;
//             //     if (c == '\n')
//             //         _state_header = H_DONE_REQUEST;
//             //     else
//             //         _state_header = H_START;
//             //     break;
//             case H_ALMOST_DONE_REQUEST:
//                 if (c == '\n')
//                     _state_header = H_DONE_REQUEST;
//                 else
//                     return 480;
//                 break;
//             case H_DONE_REQUEST:
//                 break;
//         }
//         if (_state_header == H_DONE_REQUEST) {
//             _buf_pos++;
//             _header_end = _buf_pos;
//             return 0;
//         }
//     }
//     return 1;
// }

void Request::print() {
    std::cout << "\nREQUEST LINE: \n";
    std::cout << "\'" << log::COLOR_RE;
    for (size_t i = _method_start; i < _method_end; i++) {
        std::cout << _buf[i];
    }
    std::cout << log::COLOR_NO << "\' ";
    std::cout << "\'" << log::COLOR_CY;
    for (size_t i = _uri_path_start; i < _uri_path_end; i++) {
        std::cout << _buf[i];
    }
    std::cout << log::COLOR_NO << "\' ";
    std::cout << "\'" << log::COLOR_GR;
    for (size_t i = _uri_query_start; i < _uri_query_end; i++) {
        std::cout << _buf[i];
    }
    std::cout << log::COLOR_NO << "\' ";
    std::cout << "\'" << log::COLOR_YE;
    for (size_t i = _uri_fragment_start; i < _uri_fragment_end; i++) {
        std::cout << _buf[i];
    }
    std::cout << log::COLOR_NO << "\' ";
    std::cout << "\'" << log::COLOR_BL;
    for (size_t i = _version_start; i < _version_end; i++) {
        std::cout << _buf[i];
    }
    std::cout << log::COLOR_NO << "\'";
    std::cout << "\n\n";

    if (!m_header.empty()) {
        std::cout << "HEADER [" << m_header.size() << "]:\n";
        for (std::map<std::string, std::string>::iterator it = m_header.begin();
             it != m_header.end(); it++) {
            std::cout << "\'" << log::COLOR_GR;
            std::cout << (*it).first;
            std::cout << log::COLOR_NO << "\'";
            std::cout << " = ";
            std::cout << "\'" << log::COLOR_BL;
            std::cout << (*it).second;
            std::cout << log::COLOR_NO << "\'";
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

// void Request::print() {
//     std::cout << "REQUEST: \n\'";
//     for (size_t i = 0; i < _request_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "REQUEST-LINE: \n\'";
//     for (size_t i = _method_start; i < _version_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "METHOD: \n\'";
//     for (size_t i = _method_start; i < _method_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "URI: \n\'";
//     for (size_t i = _uri_start; i < _uri_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "PATH: \n\'";
//     for (size_t i = _uri_path_start; i < _uri_path_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "QUERY: \n\'";
//     for (size_t i = _uri_query_start; i < _uri_query_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "FRAGMENT: \n\'";
//     for (size_t i = _uri_fragment_start; i < _uri_fragment_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "VERSION: \n\'";
//     for (size_t i = _version_start; i < _version_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";

//     std::cout << "HEADER: \n\'";
//     for (size_t i = _header_start; i < _header_end; i++) {
//         std::cout << _buf[i];
//     }
//     std::cout << "\'\n\n";
// }

}  // namespace core
