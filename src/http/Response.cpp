#include "Response.hpp"

#include "../utils/num_to_str.hpp"
#include "Request.hpp"
#include "status_codes.hpp"

namespace http {

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

Response::Response() : _body_type(NONE), _state(HEADER) { _header.reserve(MAX_INFO_LEN); }

Response::~Response() {}

Response::State Response::state() const { return _state; }

Response::BodyType Response::body_type() const { return _body_type; }

core::ByteBuffer &Response::header() { return _header; }

core::ByteBuffer &Response::body() { return _body; }

void Response::set_state(Response::State new_state) { _state = new_state; }

core::FileHandler &Response::file_handler() { return _file_handler; }

void Response::init() {
    _body_type = NONE;
    _state = HEADER;
    _header.clear();
    _body.clear();
}

void Response::build_error(const Request &req, int error_code) {
    _body_type = BUFFER;
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

}  // namespace http
