#include "Request.hpp"

#include <unistd.h>

#include <algorithm>

namespace core {

#define IS_METHOD_CHAR(c) (c >= 'A' && c <= 'Z')

#define MAX_BUFFER_SIZE 8192

Request::Request() : _buf_pos(0), _method(NONE), _state(REQUEST_LINE), _state_request_line(START) {
    _buf.reserve(8192);
}

Request::~Request() {}

int Request::parse_input(const char* input, std::size_t len) {
    _buf.append(input, len);
    if (_state == REQUEST_LINE) {
        int status = parse_request_line();
        if (status == 0) {
            std::cout << "DONE PARSING REQUEST LINE\n";
            _state = HEADER;
        } else if (status > 99) {
            std::cerr << "Error: " << status << '\n';
            return -1;
        } else {
            std::cout << "READ AGAIN\n";
        }
    } else {
        write(1, (char*)_buf.data() + _buf_pos, _buf.size() - _buf_pos);
    }
    return 0;
}

int Request::parse_request_line() {
    char c;
    for (ByteBuffer::iterator it = _buf.begin() + _buf_pos; it != _buf.end(); _buf_pos++, it++) {
        c = *it;
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
                std::cout << (_method_end - _method_start) << std::endl;
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
                std::cout << "found valid method: " << _method << std::endl;
                _state_request_line = AFTER_METHOD;
                break;
            case AFTER_METHOD:
                if (c == ' ')
                    break;
                _uri_start = _buf_pos;
                switch (c) {
                    case '/':
                        _state_request_line = URI_PATH;
                        break;
                    case 'h':
                        // _state_request_line = URI_HTTP;
                        // break;
                        return 1200;
                    default:
                        return 493;
                }
                break;
            case URI_PATH:
                if (c == ' ') {
                    _uri_end = _buf_pos;
                    _state_request_line = AFTER_URI;
                    break;
                } else if (!isalnum(c) && c != '-') {
                    return 494;
                }
                break;
            case URI_HTTP:
                break;
            case AFTER_URI:
                if (c == ' ')
                    break;
                _state_request_line = VERSION_H;
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
            _request_end = _buf_pos;
            return 0;
        }
    }
    return 1;
}

// /test?oekowf=ewfw#erfege

// http://123.12.12.1:80/test?oekowf=ewfw#erfege

// int Request::parse_input(const char* input, std::size_t len) {
//     size_t i = 0;
//     if (_buffer.empty()) {
//         while (i < len && input[i] == '\n')
//             i++;
//         if (i == len)
//             return 0;
//     }
//     if (_buffer.size() + len - i > MAX_BUFFER_SIZE) {
//         _status_code = 400;
//         return -1;
//     }
//     _buffer.append(&(input[i]), len - i);

//     ByteBuffer::iterator line_end =
//         std::find(_buffer.begin() + _buffer_parsed_chars, _buffer.end(), '\n');
//     if (line_end == _buffer.end())
//         return 0;
//     if (_parse_state == REQUEST_LINE) {
//         _parse_state = HEADER;
//         return parse_request_line(_buffer.begin() + _buffer_parsed_chars, line_end);
//     } else if (_parse_state == HEADER) {
//         std::cerr << "parse_headers()\n";
//     } else if (_parse_state == BODY) {
//         std::cerr << "parse_body()\n";
//     }
//     return 0;
// }

// bool is_space(const char& c) { return c == ' '; }

// int Request::parse_request_line(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
//     std::cerr << "parse_request_line()\n";
//     ByteBuffer::iterator space = std::find(begin, end, ' ');
//     if (space == end) {
//         _status_code = 400;
//         return -1;
//     }
//     if (parse_request_method(begin, space)) {
//         return -1;
//     }

//     begin = std::find_if_not(space, end, is_space);
//     if (begin == end) {
//         _status_code = 400;
//         return -1;
//     }
//     space = std::find(begin, end, ' ');
//     if (space == end) {
//         _status_code = 400;
//         return -1;
//     }
//     if (parse_request_uri(begin, space)) {
//         return -1;
//     }

//     begin = std::find_if_not(space, end, is_space);
//     if (begin == end) {
//         _status_code = 400;
//         return -1;
//     }
//     space = std::find(begin, end, ' ');
//     if (space - begin != 8 || !_buffer.equal(begin, "HTTP/1.1", 8)) {
//         _status_code = 400;
//         return -1;
//     }

//     begin = std::find_if_not(space, end, is_space);
//     if (begin != end) {
//         _status_code = 400;
//         return -1;
//     }
//     return 0;
// }

// int Request::parse_request_method(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
//     for (size_t i = 0; i < sizeof(valid_methods) / sizeof(valid_methods[0]); ++i) {
//         for (size_t j = 0; begin != end && j < valid_methods[i].first.size(); begin++, j++) {
//             if (_buffer[j] != valid_methods[i].first[j]) {
//                 break;
//             }
//         }
//         if (begin == end) {
//             if (valid_methods[i].second == NOT_IMPLEMENTED) {
//                 _status_code = 501;
//                 return -1;
//             }
//             return 0;
//         }
//     }
//     _status_code = 400;
//     return -1;
// }

// int Request::parse_request_uri_http(const ByteBuffer::iterator& begin,
//                                     const ByteBuffer::iterator& end) {
//     if (*begin == '/')
//         return 0;
//     if (!_buffer.equal(begin, "http://", 7))
//         return -1;
//     ByteBuffer::iterator it = begin + 7;
//     if (it == end)
//         return -1;
//     if (!isalnum(*it) && *it != '-')
//         return -1;
//     return 0;
// }

// int Request::parse_request_uri(const ByteBuffer::iterator& begin, const ByteBuffer::iterator&
// end) {
//     if (parse_request_uri_http(begin, end)) {
//         _status_code = 401;
//         return -1;
//     }
//     for (ByteBuffer::iterator it = begin; it != end; it++) {
//         if (!isprint(*it)) {
//             _status_code = 402;
//             return -1;
//         } else if (*it == '%' &&
//                    (it + 1 == end || !isxdigit(*it + 1) || it + 2 == end || !isxdigit(*it + 2)))
//                    {
//             _status_code = 403;
//             return -1;
//         }
//     }
//     _uri = std::string(begin, end);
//     return 0;
// }

// int Request::parse_header_line(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
//     std::cout << "Header line: ";
//     while (begin != end) {
//         ++begin;
//         // line_end = std::find(_buffer.begin() + _buffer_parsed_chars, _buffer.end(), '\n');
//     }
//     return 0;
// }

}  // namespace core
