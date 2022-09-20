#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define BUFFER_SIZE 2048

#define IMPLEMENTED true
#define NOT_IMPLEMENTED false

static const std::pair<std::string, bool> valid_methods[] = {
    std::make_pair("OPTIONS", NOT_IMPLEMENTED), std::make_pair("GET", IMPLEMENTED),
    std::make_pair("HEAD", IMPLEMENTED),        std::make_pair("POST", IMPLEMENTED),
    std::make_pair("PUT", NOT_IMPLEMENTED),     std::make_pair("DELETE", IMPLEMENTED),
    std::make_pair("TRACE", NOT_IMPLEMENTED),   std::make_pair("CONNECT", NOT_IMPLEMENTED)};

namespace core {

enum Methods { GET = 1, HEAD = 2, POST = 3, DELETE = 5 };

class Request {
   private:
    std::string                        _string;
    int                                _method;
    std::string                        _uri;
    std::map<std::string, std::string> _header;
    int                                _status_code;

    int check_version(const char *version);
    int check_method(const char *method);
    int check_uri(const char *uri);

   public:
    Request() : _status_code(0) {}
    ~Request() {}

    bool read_done() const;
    int  parse_request_line(char *req_line, const size_t &len);
    int  parse_headers();
    int  parse_body();
};

}  // namespace core

// GET / HTTP/1.1
