#include "Response.hpp"

#include "../utils/color.hpp"
#include "../utils/num_to_str.hpp"
#include "Request.hpp"
#include "mime_types.hpp"
#include "status_codes.hpp"

namespace http {

void Response::_construct_header_file(const Request &req) {
    _header.append("HTTP/1.1 ");
    _header.append(g_m_status_codes.find(200)->second.c_str());
    _header.append("\r\nServer: ");
    _header.append(SERVER_NAME);
    _header.append("\r\nContent-Type: ");
    _header.append(mime_type(_file_handler.path()));
    _header.append("\r\nContent-Length: ");
    _header.append(utils::num_to_str_dec(_file_handler.max_size()).c_str());
    _header.append("\r\nConnection: ");
    if (req.connection_should_close())
        _header.append("close");
    else
        _header.append("keep-alive");
    _header.append("\r\n\r\n");
}

void Response::_construct_header_cgi(const Request &req) {
    _header.append("HTTP/1.1 ");
    _header.append(g_m_status_codes.find(200)->second.c_str());
    _header.append("\r\nServer: ");
    _header.append(SERVER_NAME);
    _header.append("\r\nTransfer-Encoding: chunked");
    _header.append("\r\nConnection: ");
    if (req.connection_should_close())
        _header.append("close\r\n");
    else
        _header.append("keep-alive\r\n");
}

static std::map<int, error_page_t> new_error_page_default() {
    std::map<int, error_page_t> m_error_page;

    for (m_status_codes_iterator_t it = g_m_status_codes.begin(); it != g_m_status_codes.end();
         it++) {
        if (it->first >= 400) {
            error_page_t error_page;
            error_page.content.reserve(200);
            error_page.content_type = "text/html";
            error_page.content.append("<html>\r\n<head><title>");
            error_page.content.append(it->second.c_str());
            error_page.content.append("</title></head>\r\n<body>\r\n<center><h1>");
            error_page.content.append(it->second.c_str());
            error_page.content.append(
                "</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");
            m_error_page[it->first] = error_page;
        }
    }
    return m_error_page;
}

const std::map<int, error_page_t> Response::_m_error_page = new_error_page_default();

Response::Response()
    : _body_type(BODY_NONE), _state(HEADER), _cgi_pass(NULL), _is_dir_listing(false) {
    _header.reserve(MAX_INFO_LEN);
}

Response::~Response() {}

Response::State Response::state() const { return _state; }

Response::BodyType Response::body_type() const { return _body_type; }

core::ByteBuffer &Response::header() { return _header; }

core::ByteBuffer &Response::body() { return _body; }

void Response::set_state(Response::State new_state) { _state = new_state; }

core::FileHandler &Response::file_handler() { return _file_handler; }

void Response::init() {
    _body_type = BODY_NONE;
    _state = HEADER;
    _header.clear();
    _header.set_pos(0);
    _body.clear();
    _body.set_pos(0);
    _cgi_pass = NULL;
    _is_dir_listing = false;
}

const config::Redirect *Response::_find_redir(const config::Location *location,
                                              const std::string &relative_path, bool dir) {
    if (relative_path.size() == 0 && dir) {
        for (size_t i = 0; i < location->v_redirect.size(); i++) {
            if (location->v_redirect[i].origin == ".")
                return &location->v_redirect[i];
        }
    } else {
        for (size_t i = 0; i < location->v_redirect.size(); i++) {
            if (location->v_redirect[i].origin == relative_path)
                return &location->v_redirect[i];
        }
    }
    return NULL;
}

bool Response::_find_index(const config::Location *location, const std::string &absolute_path) {
    for (size_t i = 0; i < location->v_index.size(); i++) {
        try {
            _file_handler.init(absolute_path + "/" + location->v_index[i]);
            return true;
        } catch (...) {
        }
    }
    return false;
}

const config::CgiPass *Response::_find_cgi_pass(const config::Location *location,
                                                const std::string      &path) {
    size_t type_pos = path.rfind('.');
    if (type_pos != std::string::npos) {
        for (size_t i = 0; i < location->v_cgi_pass.size(); i++) {
            if (path.compare(type_pos + 1, path.size() - type_pos, location->v_cgi_pass[i].type) ==
                0) {
                return &location->v_cgi_pass[i];
            }
        }
    }
    return NULL;
}

void Response::_build_redir(const Request &req, const config::Redirect &redir) {
    _body_type = BODY_BUFFER;
    const std::string &status_code_msg = g_m_status_codes.find(redir.status_code)->second;
    _body.append("<html>\r\n<head><title>");
    _body.append(status_code_msg.c_str());
    _body.append("</title></head>\r\n<body>\r\n<center><h1>");
    _body.append(status_code_msg.c_str());
    _body.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");
    _header.append("HTTP/1.1 ");
    _header.append(status_code_msg.c_str());
    _header.append("\r\nServer: ");
    _header.append(SERVER_NAME);
    _header.append("\r\nContent-Type: text/html");
    _header.append("\r\nContent-Length: ");
    _header.append(utils::num_to_str_dec(_body.size()).c_str());
    _header.append("\r\nConnection: ");
    if (req.connection_should_close())
        _header.append("close");
    else
        _header.append("keep-alive");
    _header.append("\r\nLocation: ");
    _header.append(redir.direction.c_str());
    _header.append("\r\n\r\n");
}

void Response::build(const Request &req) {
    bool directory = !_file_handler.init(req.absolute_path());

    const config::Redirect *redir = _find_redir(req.location(), req.relative_path(), directory);
    if (redir) {
        _build_redir(req, *redir);
        return;
    }

    if (directory && !_find_index(req.location(), req.absolute_path())) {
        if (req.location()->directory_listing) {
            _body_type = BODY_CGI;
            _construct_header_cgi(req);
            _is_dir_listing = true;
            return;
        }
        throw HTTP_NOT_FOUND;
    }

    _cgi_pass = _find_cgi_pass(req.location(), _file_handler.path());
    if (_cgi_pass) {
        _body_type = BODY_CGI;
        _file_handler.close();
        _construct_header_cgi(req);
        return;
    }

    if (req.method() != Request::GET && req.method() != Request::HEAD) {
        _file_handler.close();
        throw HTTP_METHOD_NOT_ALLOWED;
    }

    if (_file_handler.max_size() == 0 || req.method() == Request::HEAD) {
        _body_type = Response::BODY_NONE;
        _file_handler.close();
    } else {
        _body_type = Response::BODY_FILE;
    }
    _construct_header_file(req);
}

void Response::build_error(const Request &req, int error_code) {
    _body_type = BODY_BUFFER;
    const error_page_t *error_page = NULL;
    if (req.server()) {
        std::map<int, error_page_t>::const_iterator it =
            req.server()->m_error_codes.find(error_code);
        if (it != req.server()->m_error_codes.end())
            error_page = &it->second;
    }
    if (!error_page) {
        std::map<int, error_page_t>::const_iterator it = _m_error_page.find(error_code);
        error_page = &it->second;
    }
    _body.assign(error_page->content.begin(), error_page->content.end());
    _header.append("HTTP/1.1 ");
    _header.append(g_m_status_codes.find(error_code)->second.c_str());
    _header.append("\r\nServer: ");
    _header.append(SERVER_NAME);
    _header.append("\r\nContent-Type: ");
    _header.append(error_page->content_type.c_str());
    _header.append("\r\nContent-Length: ");
    _header.append(utils::num_to_str_dec(_body.size()).c_str());
    _header.append("\r\nConnection: close\r\n\r\n");
}

bool Response::is_dir_listing() const { return _is_dir_listing; }

bool Response::need_cgi() const { return _body_type == BODY_CGI; }

const config::CgiPass *Response::cgi_pass() const { return _cgi_pass; }

void Response::print() const {
    std::cout
        << utils::COLOR_PL_1
        << "--------------------------------------------------------------------------------\n"
        << "RESPONSE: \n"
        << utils::COLOR_NO;

    std::cout << utils::COLOR_GR_1 << " STATUS:    " << utils::COLOR_NO;
    for (size_t i = 9; i < _header.size(); i++) {
        if (_header[i] == '\n')
            break;
        std::cout << _header[i];
    }
    std::cout << std::endl;
    std::cout << utils::COLOR_BL_1 << " BODY_TYPE: " << utils::COLOR_NO;
    switch (_body_type) {
        case BODY_NONE:
            std::cout << "NONE\n";
            break;
        case BODY_FILE:
            std::cout << "FILE\n";
            break;
        case BODY_BUFFER:
            std::cout << "BUFFER\n";
            break;
        case BODY_CGI:
            std::cout << "CGI\n";
            break;
    }
    if (_cgi_pass)
        std::cout << utils::COLOR_CY_1 << " CGI_PASS:  " << utils::COLOR_NO << _cgi_pass->path
                  << std::endl;
    else
        std::cout << utils::COLOR_CY_1 << " CGI_PASS:  " << utils::COLOR_NO << "NONE" << std::endl;
    std::cout
        << utils::COLOR_PL_1
        << "--------------------------------------------------------------------------------\n"
        << utils::COLOR_NO;
}

}  // namespace http
