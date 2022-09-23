#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"

#define IMPLEMENTED true
#define NOT_IMPLEMENTED false

// static const std::pair<std::string, bool> valid_methods[] = {
//     std::make_pair("OPTIONS", NOT_IMPLEMENTED), std::make_pair("GET", IMPLEMENTED),
//     std::make_pair("HEAD", IMPLEMENTED),        std::make_pair("POST", IMPLEMENTED),
//     std::make_pair("PUT", NOT_IMPLEMENTED),     std::make_pair("DELETE", IMPLEMENTED),
//     std::make_pair("TRACE", NOT_IMPLEMENTED),   std::make_pair("CONNECT", NOT_IMPLEMENTED)};

namespace core {

enum method { NONE, GET, HEAD, POST, DELETE };
enum state { REQUEST_LINE, HEADER, BODY };

enum state_request_line {
    START,
    METHOD,
    AFTER_METHOD,
    URI_SLASH,
    URI_ENCODE_1,
    URI_ENCODE_2,
    URI_QUERY,
    URI_FRAGMENT,
    URI_HTTP,
    AFTER_URI,
    VERSION_H,
    VERSION_HT,
    VERSION_HTT,
    VERSION_HTTP,
    VERSION_HTTP_SLASH,
    VERSION_HTTP_SLASH_MAJOR,
    VERSION_HTTP_SLASH_MAJOR_DOT,
    VERSION_HTTP_SLASH_MAJOR_DOT_MINOR,
    AFTER_VERSION,
    ALMOST_DONE,
    DONE
};

class Request {
   private:
    ByteBuffer  _buf;
    std::size_t _buf_pos;
    std::size_t _request_end;
    std::size_t _method_start;
    std::size_t _method_end;
    method      _method;
    std::size_t _version_start;
    std::size_t _version_end;

    std::size_t _uri_start;
    std::size_t _uri_end;
    std::size_t _uri_path_start;
    std::size_t _uri_path_end;
    std::size_t _uri_query_start;
    std::size_t _uri_query_end;
    std::size_t _uri_fragment_start;
    std::size_t _uri_fragment_end;

    state              _state;
    state_request_line _state_request_line;

   public:
    int status_code;

    Request();
    ~Request();

    int parse_input(const char* input, std::size_t len);
    int parse_request_line();

    void print();
};

}  // namespace core

// REQUEST LINE Parsing
// -

// HEADER Parsing
// - durch den buffer Line by line
// - check if line is too long
// - check if line starts with space, append value to last header
// - check if line contains ':'
// - extract key / value and store it in map

// BODY Parsing
// - check if body size is correct
// - append read calls to buffer
