#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"

#define IMPLEMENTED true
#define NOT_IMPLEMENTED false

static const std::pair<std::string, bool> valid_methods[] = {
    std::make_pair("OPTIONS", NOT_IMPLEMENTED), std::make_pair("GET", IMPLEMENTED),
    std::make_pair("HEAD", IMPLEMENTED),        std::make_pair("POST", IMPLEMENTED),
    std::make_pair("PUT", NOT_IMPLEMENTED),     std::make_pair("DELETE", IMPLEMENTED),
    std::make_pair("TRACE", NOT_IMPLEMENTED),   std::make_pair("CONNECT", NOT_IMPLEMENTED)};

namespace core {

enum Methods { GET = 1, HEAD = 2, POST = 3, DELETE = 5 };
enum ParseState { REQUEST_LINE, HEADER, BODY };

class Request {
   private:
    ByteBuffer                         _buffer;
    size_t                             _buffer_parsed_chars;
    ParseState                         _parse_state;
    int                                _method;
    std::string                        _uri;
    std::map<std::string, std::string> _header;

    int check_version(const char *version);
    int check_method(const char *method);
    int check_uri(const char *uri);

   public:
    int _status_code;
    Request();
    ~Request();

    int parse_input(const char *input, std::size_t len);
    int parse_request_line(ByteBuffer::iterator begin, const ByteBuffer::iterator &end);
    int parse_header_line(ByteBuffer::iterator begin, const ByteBuffer::iterator &end);
    int parse_request_method(ByteBuffer::iterator begin, const ByteBuffer::iterator &end);
    int parse_request_uri(const ByteBuffer::iterator &begin, const ByteBuffer::iterator &end);
    int parse_request_uri_http(const ByteBuffer::iterator &begin, const ByteBuffer::iterator &end);
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
