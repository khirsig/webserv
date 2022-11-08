#include "Request.hpp"

#include "../core/Address.hpp"
#include "../http/status_codes.hpp"
#include "../settings.hpp"
#include "../utils/color.hpp"
#include "../utils/str_to_num.hpp"

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

namespace http {

Request::Request()
    : _state(REQUEST_LINE),
      _state_request_line(RL_START),
      _state_header(H_KEY_START),
      _state_body_chunked(BC_LENGTH_START),
      _info_len(0),
      _chunk_len(0),
      _method(NONE),
      _body_content_type(CONT_NONE),
      _content_len(0),
      _connection(CONN_KEEP_ALIVE),
      _server(NULL),
      _location(NULL),
      MAX_METHOD_LEN(7) {
    _method_str.reserve(MAX_METHOD_LEN);
    _path_encoded.reserve(MAX_INFO_LEN / 4);
    _path_decoded.reserve(MAX_INFO_LEN / 4);
    _query_string.reserve(MAX_INFO_LEN / 4);
    _host_encoded.reserve(MAX_INFO_LEN / 4);
    _host_decoded.reserve(MAX_INFO_LEN / 4);
    _key.reserve(MAX_INFO_LEN / 4);
    _value.reserve(MAX_INFO_LEN / 4);
}

Request::~Request() {}

void Request::init() {
    _state = REQUEST_LINE;
    _state_request_line = RL_START;
    _state_header = H_KEY_START;
    _state_body_chunked = BC_LENGTH_START;
    _info_len = 0;
    _chunk_len = 0;
    _method = NONE;
    _body_content_type = CONT_NONE;
    _content_len = 0;
    _connection = CONN_KEEP_ALIVE;
    _server = NULL;
    _location = NULL;
    _method_str.clear();
    _path_encoded.clear();
    _path_decoded.clear();
    _query_string.clear();
    _host_encoded.clear();
    _host_decoded.clear();
    _key.clear();
    _value.clear();
    _m_header.clear();
    _body.clear();
}

bool Request::parse(const char *buf, size_t buf_len, size_t &buf_pos,
                    const std::vector<config::Server> &v_server, const core::Address &socket_addr) {
    if (_state == REQUEST_LINE) {
        if (!_parse_request_line(buf, buf_len, buf_pos))
            return false;
        _analyze_request_line();
        _state = HEADER;
    }
    if (_state == HEADER) {
        if (!_parse_header(buf, buf_len, buf_pos))
            return false;
        _analyze_header();
        _find_server(v_server, socket_addr);
        _find_location();
        _process_path();
        if (_body_content_type == CONT_LENGTH && _location->client_max_body_size < _content_len)
            throw HTTP_CONTENT_TOO_LARGE;
        _body.reserve(_content_len);
        switch (_body_content_type) {
            case CONT_LENGTH:
                _state = BODY;
                break;
            case CONT_CHUNKED:
                _state = BODY_CHUNKED;
                break;
            case CONT_NONE:
                _state = DONE;
                break;
        }
    }
    if (_state == BODY) {
        size_t left_len = _content_len - _body.size();
        if (left_len > buf_len - buf_pos)
            left_len = buf_len - buf_pos;
        _body.append(buf + buf_pos, left_len);
        if (_body.size() != _content_len)
            return false;
        _state = DONE;
    }
    if (_state == BODY_CHUNKED) {
        if (!_parse_body_chunked(buf, buf_len, buf_pos))
            return false;
        _state = DONE;
    }
    if (_state == DONE) {
#if PRINT_LEVEL > 1
        print();
#endif
        return true;
    }
    return false;
}

bool Request::_parse_request_line(const char *buf, size_t buf_len, size_t &buf_pos) {
    char c;
    for (; buf_pos < buf_len; buf_pos++, _info_len++) {
        if (_info_len > MAX_INFO_LEN)
            throw HTTP_BAD_REQUEST;
        c = buf[buf_pos];
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
                if (IS_METHOD_CHAR(c) && _method_str.size() < MAX_METHOD_LEN) {
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
        if (_state_request_line == RL_DONE) {
            buf_pos++;
            return true;
        }
    }
    return false;
}

void Request::_parse_method() {
    switch (_method_str.size()) {
        case 3:
            if (_method_str == "GET") {
                _method = Request::GET;
                return;
            }
            if (_method_str == "PUT")
                throw HTTP_NOT_IMPLEMENTED;
            break;
        case 4:
            if (_method_str == "HEAD") {
                _method = Request::HEAD;
                return;
            }
            if (_method_str == "POST") {
                _method = Request::POST;
                return;
            }
            break;
        case 5:
            if (_method_str == "PATCH")
                throw HTTP_NOT_IMPLEMENTED;
            if (_method_str == "TRACE")
                throw HTTP_NOT_IMPLEMENTED;
            break;
        case 6:
            if (_method_str == "DELETE") {
                _method = Request::DELETE;
                return;
            }
            break;
        case 7:
            if (_method_str == "CONNECT")
                throw HTTP_NOT_IMPLEMENTED;
            if (_method_str == "OPTIONS")
                throw HTTP_NOT_IMPLEMENTED;
            break;
        default:
            break;
    }
    throw HTTP_BAD_REQUEST;
}

static void uri_decode(std::string &src, std::string &dest) {
    dest.reserve(src.size());
    char c, c_decoded;
    enum StateUriDecode { CHAR, HEX_1, HEX_2 };
    StateUriDecode state = CHAR;
    for (size_t i = 0; i < src.size(); i++) {
        c = src[i];
        switch (state) {
            case CHAR:
                switch (c) {
                    case '%':
                        state = HEX_1;
                        break;
                    default:
                        dest += c;
                        break;
                }
                break;
            case HEX_1:
                c_decoded = HEX_CHAR_TO_INT(c);
                state = HEX_2;
                break;
            case HEX_2:
                c_decoded = c_decoded * 16 + HEX_CHAR_TO_INT(c);
                dest += c_decoded;
                state = CHAR;
                break;
        }
    }
}

static void uri_path_depth_check(std::string &path) {
    char c;
    int  depth = 0;
    enum StatePathCheck { SLASH, SEGMENT, DOT_1, DOT_2 };
    StatePathCheck state = SLASH;
    for (std::size_t i = 0; i < path.size(); i++) {
        c = path[i];
        switch (state) {
            case SLASH:
                switch (c) {
                    case '/':
                        break;
                    case '.':
                        state = DOT_1;
                        break;
                    default:
                        state = SEGMENT;
                        break;
                }
                break;
            case SEGMENT:
                switch (c) {
                    case '/':
                        depth++;
                        state = SLASH;
                        break;
                    default:
                        break;
                }
                break;
            case DOT_1:
                switch (c) {
                    case '/':
                        state = SLASH;
                        break;
                    case '.':
                        state = DOT_2;
                        break;
                    default:
                        state = SEGMENT;
                        break;
                }
                break;
            case DOT_2:
                switch (c) {
                    case '/':
                        depth--;
                        if (depth < 0)
                            throw HTTP_BAD_REQUEST;
                        state = SLASH;
                        break;
                    default:
                        state = SEGMENT;
                        break;
                }
                break;
        }
    }
    if (state == DOT_2 && depth == 0) {
        throw HTTP_BAD_REQUEST;
    }
}

void Request::_analyze_request_line() {
    uri_decode(_path_encoded, _path_decoded);
    uri_decode(_host_encoded, _host_decoded);
    uri_path_depth_check(_path_decoded);
}

void Request::_analyze_header() {
    typedef std::map<std::string, std::string>::const_iterator const_header_it;

    bool host_found = false;
    for (const_header_it it = _m_header.begin(); it != _m_header.end(); it++) {
        if (it->first == "HOST") {
            if (it->second.size() == 0)
                throw HTTP_BAD_REQUEST;
            host_found = true;
            if (_host_decoded.size() == 0)
                _host_decoded = it->second;
        } else if (it->first == "CONTENT-LENGTH") {
            if (_body_content_type != CONT_NONE)
                throw HTTP_BAD_REQUEST;
            _body_content_type = CONT_LENGTH;
            if (!utils::str_to_num_dec(it->second, _content_len))
                throw HTTP_BAD_REQUEST;
        } else if (it->first == "TRANSFER-ENCODING") {
            if (it->second == "chunked") {
                if (_body_content_type != CONT_NONE)
                    throw HTTP_BAD_REQUEST;
                _body_content_type = CONT_CHUNKED;
            } else {
                throw HTTP_NOT_IMPLEMENTED;
            }
        } else if (it->first == "CONNECTION") {
            if (it->second == "close") {
                _connection = CONN_CLOSE;
            }
        }
    }
    if (!host_found)
        throw HTTP_BAD_REQUEST;
}

void Request::_find_server(const std::vector<config::Server> &v_server,
                           const core::Address               &socket_addr) {
    typedef std::vector<config::Server>::const_iterator const_server_it;

    _server = NULL;
    for (const_server_it it = v_server.begin(); it != v_server.end(); it++) {
        for (std::size_t i = 0; i < it->v_listen.size(); i++) {
            if ((it->v_listen[i].addr == INADDR_ANY || it->v_listen[i].addr == socket_addr.addr) &&
                it->v_listen[i].port == socket_addr.port) {
                if (_server == NULL) {
                    _server = &(*it);
                } else {
                    for (std::size_t j = 0; j < it->v_server_name.size(); j++) {
                        if (it->v_server_name[j] == _host_decoded) {
                            _server = &(*it);
                        }
                    }
                }
                break;
            }
        }
    }
    if (_server == NULL)
        throw HTTP_INTERNAL_SERVER_ERROR;
}

void Request::_find_location() {
    _location = NULL;
    for (std::size_t i = 0; i < _server->v_location.size(); i++) {
        if (_path_decoded.find(_server->v_location[i].path) == 0) {
            if (_location == NULL || _location->path.size() < _server->v_location[i].path.size())
                _location = &(_server->v_location[i]);
        }
    }
    if (_location == NULL)
        throw HTTP_NOT_FOUND;
}

void Request::_process_path() {
    const std::string &uri_path = _path_decoded;
    size_t             uri_path_offset;

    if (_location->path.size() == 1)
        uri_path_offset = 1;
    else if (_location->path.size() == uri_path.size())
        uri_path_offset = _location->path.size();
    else
        uri_path_offset = _location->path.size() + 1;
    _relative_path.assign(uri_path.begin() + uri_path_offset, uri_path.end());
    _absolute_path = _location->root + _relative_path;
}

void Request::_add_header() {
    std::transform(_key.begin(), _key.end(), _key.begin(), ::toupper);
    std::pair<std::map<std::string, std::string>::iterator, bool> pair =
        _m_header.insert(std::make_pair(_key, _value));
    if (pair.second == false) {
        if (pair.first->first == "HOST")
            throw HTTP_BAD_REQUEST;
        pair.first->second = pair.first->second + ", " + _value;
    }
    _key.clear();
    _value.clear();
}

bool Request::_parse_header(const char *buf, size_t buf_len, size_t &buf_pos) {
    char c;
    for (; buf_pos < buf_len; buf_pos++, _info_len++) {
        if (_info_len > MAX_INFO_LEN)
            throw HTTP_BAD_REQUEST;
        c = buf[buf_pos];
        switch (_state_header) {
            case H_KEY_START:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_HEADER;
                        break;
                    case '\n':
                        buf_pos++;
                        return true;
                    default:
                        if (!IS_TOKEN_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _key += c;
                        _state_header = H_KEY;
                        break;
                }
                break;
            case H_KEY:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_HEADER_LINE;
                        break;
                    case '\n':
                        _state_header = H_KEY_START;
                        _add_header();
                        break;
                    case ':':
                        _state_header = H_VALUE_START;
                        break;
                    default:
                        if (!IS_TOKEN_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _key += c;
                        break;
                }
                break;
            case H_VALUE_START:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_HEADER_LINE;
                        break;
                    case '\n':
                        _state_header = H_KEY_START;
                        _add_header();
                        break;
                    case '\t':
                    case ' ':
                        break;
                    default:
                        if (!IS_TEXT_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _value += c;
                        _state_header = H_VALUE;
                        break;
                }
                break;
            case H_VALUE:
                switch (c) {
                    case '\r':
                        _state_header = H_ALMOST_DONE_HEADER_LINE;
                        break;
                    case '\n':
                        _state_header = H_KEY_START;
                        _add_header();
                        break;
                    default:
                        if (!IS_TEXT_CHAR(c))
                            throw HTTP_BAD_REQUEST;
                        _value += c;
                        break;
                }
                break;
            case H_ALMOST_DONE_HEADER_LINE:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                _state_header = H_KEY_START;
                _add_header();
                break;
            case H_ALMOST_DONE_HEADER:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                buf_pos++;
                return true;
        }
    }
    return false;
}

bool Request::_parse_body_chunked(const char *buf, size_t buf_len, size_t &buf_pos) {
    char c;

    for (; buf_pos < buf_len; buf_pos++) {
        c = buf[buf_pos];
        switch (_state_body_chunked) {
            case BC_LENGTH_START:
                switch (c) {
                    case '0':
                        _state_body_chunked = BC_LENGTH_0;
                        break;
                    default:
                        if (!isxdigit(c))
                            throw HTTP_BAD_REQUEST;
                        _state_body_chunked = BC_LENGTH;
                        _chunk_len = HEX_CHAR_TO_INT(c);
                        break;
                }
                break;
            case BC_LENGTH:
                switch (c) {
                    case '\r':
                        _state_body_chunked = BC_LENGTH_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_body_chunked = BC_DATA;
                        break;
                    case ';':
                        _state_body_chunked = BC_LENGTH_EXTENSION;
                        break;
                    default:
                        if (!isxdigit(c))
                            throw HTTP_BAD_REQUEST;
                        _chunk_len = _chunk_len * 16 + HEX_CHAR_TO_INT(c);
                        if (_body.size() + _chunk_len > _location->client_max_body_size)
                            throw HTTP_CONTENT_TOO_LARGE;
                        break;
                }
                break;
            case BC_LENGTH_EXTENSION:
                switch (c) {
                    case '\r':
                        _state_body_chunked = BC_LENGTH_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_body_chunked = BC_DATA;
                        break;
                    default:
                        break;
                }
                break;
            case BC_LENGTH_ALMOST_DONE:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                _state_body_chunked = BC_DATA;
                break;
            case BC_DATA:
                if (_chunk_len > 0) {
                    _body += c;
                    _chunk_len--;
                    break;
                }
                switch (c) {
                    case '\r':
                        _state_body_chunked = BC_DATA_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_body_chunked = BC_LENGTH_START;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case BC_DATA_ALMOST_DONE:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                _state_body_chunked = BC_LENGTH_START;
                break;
            case BC_LENGTH_0:
                switch (c) {
                    case '\r':
                        _state_body_chunked = BC_LENGTH_0_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_body_chunked = BC_DATA_0;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case BC_LENGTH_0_ALMOST_DONE:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                _state_body_chunked = BC_DATA_0;
                break;
            case BC_DATA_0:
                switch (c) {
                    case '\r':
                        _state_body_chunked = BC_ALMOST_DONE;
                        break;
                    case '\n':
                        _state_body_chunked = BC_DONE;
                        break;
                    default:
                        throw HTTP_BAD_REQUEST;
                }
                break;
            case BC_ALMOST_DONE:
                if (c != '\n')
                    throw HTTP_BAD_REQUEST;
                _state_body_chunked = BC_DONE;
                break;
            case BC_DONE:
                break;
        }
        if (_state_body_chunked == BC_DONE) {
            buf_pos++;
            return true;
        }
    }
    return false;
}

void Request::print() const {
    typedef std::map<std::string, std::string>::const_iterator const_header_it;
    std::cout
        << utils::COLOR_PL_1
        << "--------------------------------------------------------------------------------\n"
        << "REQUEST: \n"
        << utils::COLOR_NO;
    std::cout << utils::COLOR_GR_1 << " REQUEST LINE:\n" << utils::COLOR_NO;
    std::cout << utils::COLOR_GR << "  - METHOD:   " << utils::COLOR_NO << _method_str << "\n";
    std::cout << utils::COLOR_GR << "  - PATH:     " << utils::COLOR_NO << _path_decoded << "\n";
    std::cout << utils::COLOR_GR << "  - REL_PATH: " << utils::COLOR_NO << _relative_path << "\n";
    std::cout << utils::COLOR_GR << "  - ABS_PATH: " << utils::COLOR_NO << _absolute_path << "\n";
    std::cout << utils::COLOR_GR << "  - QUERY:    " << utils::COLOR_NO << _query_string << "\n";
    std::cout << utils::COLOR_GR << "  - HOST:     " << utils::COLOR_NO << _host_decoded << "\n";
    std::cout << utils::COLOR_BL_1 << " HEADER:\n" << utils::COLOR_NO;
    for (const_header_it it = _m_header.begin(); it != _m_header.end(); it++)
        std::cout << utils::COLOR_BL << "  - " << it->first << ": " << utils::COLOR_NO << it->second
                  << "\n";
    std::cout << utils::COLOR_CY_1 << " BODY (" << utils::COLOR_NO << _body.size()
              << utils::COLOR_CY_1 << "):" << utils::COLOR_NO << "\n";
    std::cout << utils::COLOR_CY << "  \'" << utils::COLOR_NO;
    for (size_t i = 0; i < _body.size(); i++) {
        if (i > 0 && i % 75 == 0)
            std::cout << "\n   ";
        std::cout << _body[i];
    }
    std::cout << utils::COLOR_CY << "\'\n" << utils::COLOR_NO;
    std::cout
        << utils::COLOR_PL_1
        << "--------------------------------------------------------------------------------\n"
        << utils::COLOR_NO;
}

bool Request::connection_should_close() const { return _connection == CONN_CLOSE; }

Request::Method Request::method() const { return _method; }

const std::string &Request::method_str() const { return _method_str; }

const std::string &Request::path_encoded() const { return _path_encoded; }

const std::string &Request::path_decoded() const { return _path_decoded; }

const std::string &Request::query_string() const { return _query_string; }

const std::string &Request::host_encoded() const { return _host_encoded; }

const std::string &Request::host_decoded() const { return _host_decoded; }

const std::map<std::string, std::string> &Request::m_header() const { return _m_header; }

const config::Server *Request::server() const { return _server; }

const config::Location *Request::location() const { return _location; }

const core::ByteBuffer &Request::body() const { return _body; }

const std::string &Request::relative_path() const { return _relative_path; }

const std::string &Request::absolute_path() const { return _absolute_path; }

}  // namespace http
