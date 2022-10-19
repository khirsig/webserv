#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../config/Location.hpp"
#include "../config/Server.hpp"
#include "../core/ByteBuffer.hpp"

#define IMPLEMENTED true
#define NOT_IMPLEMENTED false

#define WEBSERV_OK 0
#define WEBSERV_AGAIN -1

// static const std::pair<std::string, bool> valid_methods[] = {
//     std::make_pair("OPTIONS", NOT_IMPLEMENTED), std::make_pair("GET", IMPLEMENTED),
//     std::make_pair("HEAD", IMPLEMENTED),        std::make_pair("POST", IMPLEMENTED),
//     std::make_pair("PUT", NOT_IMPLEMENTED),     std::make_pair("DELETE", IMPLEMENTED),
//     std::make_pair("TRACE", NOT_IMPLEMENTED),   std::make_pair("CONNECT", NOT_IMPLEMENTED)};

namespace http {

// enum method { NONE, GET, HEAD, POST, DELETE };

enum state { REQUEST_LINE, REQUEST_HEADER, REQUEST_BODY, REQUEST_BODY_CHUNKED, REQUEST_DONE };

enum state_request_line {
    START,
    METHOD,
    AFTER_METHOD,
    URI_HT,
    URI_HTT,
    URI_HTTP,
    URI_HTTP_COLON,
    URI_HTTP_COLON_SLASH,
    URI_HTTP_COLON_SLASH_SLASH,
    URI_HTTP_COLON_SLASH_SLASH_HOST,
    URI_HOST_ENCODE_1,
    URI_HOST_ENCODE_2,
    URI_HOST_PORT,
    URI_SLASH,
    URI_ENCODE_1,
    URI_ENCODE_2,
    URI_QUERY,
    URI_FRAGMENT,
    AFTER_URI,
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

enum state_header {
    H_KEY_START,
    H_KEY,
    H_VALUE_START,
    H_VALUE,
    H_ALMOST_DONE_HEADER_LINE,
    H_ALMOST_DONE_HEADER
};

enum state_chunked_body {
    CHUNKED_BODY_LENGTH_START,
    CHUNKED_BODY_LENGTH,
    CHUNKED_BODY_LENGTH_EXTENSION,
    CHUNKED_BODY_LENGTH_ALMOST_DONE,
    CHUNKED_BODY_DATA_SKIP,
    // CHUNKED_BODY_AFTER_DATA,
    CHUNKED_BODY_DATA_ALMOST_DONE,
    CHUNKED_BODY_LENGTH_0,
    CHUNKED_BODY_LENGTH_0_ALMOST_DONE,
    CHUNKED_BODY_DATA_0,
    CHUNKED_ALMOST_DONE,
    CHUNKED_DONE
};

enum CONNECTION_STATE { CONNECTION_CLOSE, CONNECTION_KEEP_ALIVE };

class Request {
   public:
    core::ByteBuffer& _buf;

    std::size_t _request_end;
    std::size_t _method_start;
    std::size_t _method_end;
    std::string _method;
    std::size_t _version_start;
    std::size_t _version_end;

    std::size_t _uri_start;
    std::size_t _uri_end;
    std::size_t _uri_host_start;
    std::size_t _uri_host_end;
    std::size_t _uri_port_start;
    std::size_t _uri_port_end;
    std::size_t _uri_path_start;
    std::size_t _uri_path_end;
    std::size_t _uri_query_start;
    std::size_t _uri_query_end;
    std::size_t _uri_fragment_start;
    std::size_t _uri_fragment_end;

    std::size_t _header_start;
    std::size_t _header_end;

    std::size_t _header_key_start;
    std::size_t _header_key_end;
    std::size_t _header_value_start;
    std::size_t _header_value_end;

    bool               _chunked_body;
    core::ByteBuffer   _chunked_body_buf;
    state_chunked_body _chunked_body_state;
    std::size_t        _chunk_size;
    std::size_t        _body_expected_size;
    std::size_t        _body_start;
    std::size_t        _body_end;

    CONNECTION_STATE connection_state;

    state              _state;
    state_request_line _state_request_line;
    state_header       _state_header;

    std::string _uri_host_decoded;
    std::string _uri_path_decoded;

    std::map<std::string, std::string> _m_header;

    // config::Server&   _server;
    // config::Location& _location;

    void _uri_decode(const core::ByteBuffer& buf, std::size_t start, std::size_t end,
                     std::string& res);
    void _add_header();
    void _parse_method();
    void _analyze_request_line();
    void _analyze_header();
    void _uri_path_depth_check();

   public:
    int error;

    Request(core::ByteBuffer& buf);
    ~Request();

    void parse_input();
    int  parse_request_line();
    int  parse_header();
    int  parse_chunked_body();

    void print();
    bool done();
};

}  // namespace http
