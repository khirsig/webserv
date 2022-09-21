#include "Request.hpp"

#include <algorithm>

namespace core {

#define MAX_BUFFER_SIZE 8192

Request::Request() : _buffer_parsed_chars(0), _parse_state(REQUEST_LINE), _status_code(0) {
    _buffer.reserve(4096);
}

Request::~Request() {}

int Request::parse_input(const char* input, std::size_t len) {
    size_t i = 0;
    if (_buffer.empty()) {
        while (i < len && input[i] == '\n')
            i++;
        if (i == len)
            return 0;
    }
    if (_buffer.size() + len - i > MAX_BUFFER_SIZE) {
        _status_code = 400;
        return -1;
    }
    _buffer.append(&(input[i]), len - i);

    ByteBuffer::iterator line_end =
        std::find(_buffer.begin() + _buffer_parsed_chars, _buffer.end(), '\n');
    if (line_end == _buffer.end())
        return 0;
    if (_parse_state == REQUEST_LINE) {
        _parse_state = HEADER;
        return parse_request_line(_buffer.begin() + _buffer_parsed_chars, line_end);
    } else if (_parse_state == HEADER) {
        std::cerr << "parse_headers()\n";
    } else if (_parse_state == BODY) {
        std::cerr << "parse_body()\n";
    }
    return 0;
}

bool is_space(const char& c) { return c == ' '; }

int Request::parse_request_line(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
    std::cerr << "parse_request_line()\n";
    ByteBuffer::iterator space = std::find(begin, end, ' ');
    if (space == end) {
        _status_code = 400;
        return -1;
    }
    if (parse_request_method(begin, space)) {
        return -1;
    }

    begin = std::find_if_not(space, end, is_space);
    if (begin == end) {
        _status_code = 400;
        return -1;
    }
    space = std::find(begin, end, ' ');
    if (space == end) {
        _status_code = 400;
        return -1;
    }
    if (parse_request_uri(begin, space)) {
        return -1;
    }

    begin = std::find_if_not(space, end, is_space);
    if (begin == end) {
        _status_code = 400;
        return -1;
    }
    space = std::find(begin, end, ' ');
    if (space - begin != 8 || !_buffer.equal(begin, "HTTP/1.1", 8)) {
        _status_code = 400;
        return -1;
    }

    begin = std::find_if_not(space, end, is_space);
    if (begin != end) {
        _status_code = 400;
        return -1;
    }
    return 0;
}

int Request::parse_request_method(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
    for (size_t i = 0; i < sizeof(valid_methods) / sizeof(valid_methods[0]); ++i) {
        for (size_t j = 0; begin != end && j < valid_methods[i].first.size(); begin++, j++) {
            if (_buffer[j] != valid_methods[i].first[j]) {
                break;
            }
        }
        if (begin == end) {
            if (valid_methods[i].second == NOT_IMPLEMENTED) {
                _status_code = 501;
                return -1;
            }
            return 0;
        }
    }
    _status_code = 400;
    return -1;
}

int Request::parse_request_uri_http(const ByteBuffer::iterator& begin,
                                    const ByteBuffer::iterator& end) {
    if (*begin == '/')
        return 0;
    if (!_buffer.equal(begin, "http://", 7))
        return -1;
    ByteBuffer::iterator it = begin + 7;
    if (it == end)
        return -1;
    if (!isalnum(*it) && *it != '-')
        return -1;
    return 0;
}

int Request::parse_request_uri(const ByteBuffer::iterator& begin, const ByteBuffer::iterator& end) {
    if (parse_request_uri_http(begin, end)) {
        _status_code = 401;
        return -1;
    }
    for (ByteBuffer::iterator it = begin; it != end; it++) {
        if (!isprint(*it)) {
            _status_code = 402;
            return -1;
        } else if (*it == '%' &&
                   (it + 1 == end || !isxdigit(*it + 1) || it + 2 == end || !isxdigit(*it + 2))) {
            _status_code = 403;
            return -1;
        }
    }
    _uri = std::string(begin, end);
    return 0;
}

int Request::parse_header_line(ByteBuffer::iterator begin, const ByteBuffer::iterator& end) {
    std::cout << "Header line: ";
    while (begin != end) {
        ++begin;
        // line_end = std::find(_buffer.begin() + _buffer_parsed_chars, _buffer.end(), '\n');
    }
    return 0;
}

}  // namespace core
