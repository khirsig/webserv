#include "Connection.hpp"

#include "../http/status_codes.hpp"
#include "../utils/addr_to_str.hpp"
#include "../utils/color.hpp"
#include "../utils/get_cwd.hpp"
#include "../utils/num_to_str.hpp"
#include "Webserver.hpp"

namespace core {

void Connection::_build_cgi_env() {
    typedef std::map<std::string, std::string>::iterator map_it;

    _request.m_header().insert(std::make_pair("AUTH_TYPE", ""));
    map_it it = _request.m_header().find("CONTENT-LENGTH");
    if (it != _request.m_header().end())
        _request.m_header().insert(std::make_pair("CONTENT_LENGTH", it->second));
    it = _request.m_header().find("CONTENT-TYPE");
    if (it != _request.m_header().end())
        _request.m_header().insert(std::make_pair("CONTENT_TYPE", it->second));
    _request.m_header().insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
    _request.m_header().insert(std::make_pair("PATH_INFO", ""));
    _request.m_header().insert(std::make_pair("QUERY_STRING", _request.query_string()));
    _request.m_header().insert(std::make_pair("REMOTE_ADDR", utils::addr_to_str(_client_addr)));
    _request.m_header().insert(std::make_pair("REMOTE_HOST", utils::addr_to_str(_client_addr)));
    _request.m_header().insert(std::make_pair("REQUEST_METHOD", _request.method_str()));
    _request.m_header().insert(std::make_pair("SCRIPT_NAME", _response.cgi_script_path()));
    _request.m_header().insert(std::make_pair("SERVER_NAME", ""));
    _request.m_header().insert(
        std::make_pair("SERVER_PORT", utils::num_to_str_dec(ntohs(_socket_addr.port))));
    _request.m_header().insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
    _request.m_header().insert(std::make_pair("SERVER_SOFTWARE", SERVER_NAME));
    // PHP specific
    _request.m_header().insert(std::make_pair("REDIRECT_STATUS", "200"));
    _request.m_header().insert(std::make_pair("SCRIPT_FILENAME", _response.cgi_script_path()));
    _request.m_header().insert(std::make_pair("DOCUMENT_ROOT", _request.location()->root));
}

const std::string Connection::_max_pipe_size_str = utils::num_to_str_hex(MAX_PIPE_SIZE);

Connection::Connection()
    : _fd(-1),
      _buf_pos(0),
      _buf_filled(0),
      _cgi_handler(_request, _response),
      BUF_SIZE(CONNECTION_BUF_SIZE) {
    _buf = new char[BUF_SIZE];
}

Connection::~Connection() { delete[] _buf; }

bool Connection::is_active() const { return _is_active; }

bool Connection::is_request_done() const { return _is_request_done; }

bool Connection::is_response_done() const { return _is_response_done; }

bool Connection::should_close() const { return _should_close; }

int Connection::fd() const { return _fd; }

void Connection::init(int fd, Address client_addr, Address socket_addr) {
    _fd = fd;
    _buf_pos = 0;
    _buf_filled = 0;
    _client_addr = client_addr;
    _socket_addr = socket_addr;
    _should_close = false;
    _is_active = false;
    _is_request_done = false;
    _is_response_done = false;
    _request_error = 0;
    _request.init();
    _response.init();
    _cgi_handler.init(_fd);

#if PRINT_LEVEL > 0
    std::cout << utils::COLOR_BL << "[Accepted]: " << utils::COLOR_NO
              << utils::addr_to_str(_client_addr) << std::endl;
#endif
}

void Connection::reinit() {
    _buf_pos = 0;
    _buf_filled = 0;
    _should_close = false;
    _is_active = false;
    _is_request_done = false;
    _is_response_done = false;
    _request_error = 0;
    _request.init();
    _response.init();
    _cgi_handler.init(_fd);
}

void Connection::receive(size_t data_len) {
    _buf_pos = 0;
    size_t to_recv_len = data_len < BUF_SIZE ? data_len : BUF_SIZE;
    _buf_filled = recv(_fd, _buf, to_recv_len, 0);
    if (_buf_filled != to_recv_len) {
        throw std::runtime_error("recv: failed");
    }
#if PRINT_LEVEL > 2
    std::cout << utils::COLOR_BL << "[Received]: " << utils::COLOR_NO
              << utils::num_to_str_dec(_buf_filled) << " bytes" << std::endl;
#endif
}

void Connection::parse_request(const std::vector<config::Server>& v_server) {
    if (_buf_pos == _buf_filled)
        return;
    _is_active = true;
    try {
        _is_request_done = _request.parse(_buf, _buf_filled, _buf_pos, v_server, _socket_addr);
        if (_is_request_done) {
            if (_request.connection_should_close())
                _should_close = true;
        }
    } catch (int error) {
        _request_error = error;
        _is_request_done = true;
        _should_close = true;
    }
}

void Connection::build_response(EventNotificationInterface& eni) {
    if (!_request_error) {
        try {
            _response.build(_request);
            if (_response.is_dir_listing())
                _cgi_handler.execute(eni, utils::get_absolute_path(DIR_LISTING_CGI_PATH),
                                     utils::get_absolute_path(DIR_LISTING_CGI_SCRIPT_PATH));
            else if (_response.need_cgi()) {
                _build_cgi_env();
                _cgi_handler.execute(eni, _response.cgi_pass()->path, _response.cgi_script_path());
            }
        } catch (int error) {
            if (error == HTTP_NOT_FOUND || error == HTTP_FORBIDDEN)
                _should_close = false;
            else
                _should_close = true;
            _response.build_error(_request, error);
        }
    } else {
        if (_request_error == HTTP_NOT_FOUND || _request_error == HTTP_FORBIDDEN)
            _should_close = false;
        else
            _should_close = true;
        _response.build_error(_request, _request_error);
    }
#if PRINT_LEVEL > 1
    _response.print();
#endif
}

bool Connection::send_response(EventNotificationInterface& eni, size_t max_len) {
    size_t left_len;
    size_t to_send_len;
    size_t pos;
    if (_response.state() == http::Response::HEADER) {
        pos = _response.header().pos();
        left_len = _response.header().size() - pos;
        to_send_len = left_len < max_len ? left_len : max_len;
        size_t sent_len = send(_fd, &(_response.header()[pos]), to_send_len, 0);
        if (sent_len != to_send_len)
            throw std::runtime_error("send: failed");
        pos += sent_len;
        _response.header().set_pos(pos);
        max_len -= sent_len;
        if (pos >= _response.header().size()) {
            if (_response.body_type() == http::Response::BODY_NONE) {
                _response.set_state(http::Response::DONE);
            } else if (_response.body_type() == http::Response::BODY_CGI) {
                _response.set_state(http::Response::HEADER_CGI);
            } else {
                _response.set_state(http::Response::BODY);
            }
        }
    }
    if (_response.state() == http::Response::HEADER_CGI && max_len > 0) {
        // if (_response.body().size() < 2) {
        //     eni.disable_event(_fd, EVFILT_WRITE);
        //     return;
        // }

        char needle_1[] = "\r\n\r\n";
        char needle_2[] = "\n\n";

        core::ByteBuffer::const_iterator start_needle_1 =
            std::search(_response.body().begin(), _response.body().end(), needle_1,
                        needle_1 + sizeof(needle_1) - 1);
        core::ByteBuffer::const_iterator start_needle_2 =
            std::search(_response.body().begin(), _response.body().end(), needle_2,
                        needle_2 + sizeof(needle_2) - 1);
        core::ByteBuffer::const_iterator cgi_header_end;
        bool                             found_needle = false;
        if (start_needle_1 < start_needle_2) {
            cgi_header_end = start_needle_1 + sizeof(needle_1) - 1;
            found_needle = true;
        } else if (start_needle_2 < start_needle_1) {
            cgi_header_end = start_needle_2 + sizeof(needle_2) - 1;
            found_needle = true;
        } else {
            cgi_header_end = _response.body().end();
            found_needle = false;
        }

        // std::cerr << "found needle: " << found_needle << std::endl;
        // std::cerr << "cgi done: " << _cgi_handler.is_done() << std::endl;
        // std::cerr << "body size: " << _response.body().size() << std::endl;
        // std::cerr << "body: \'" << _response.body() << "\'" << std::endl;
        if (!found_needle) {
            eni.disable_event(_fd, EVFILT_WRITE);
            eni.delete_event(_fd, EVFILT_TIMER);
            return false;
        }

        pos = _response.body().pos();
        left_len = cgi_header_end - (_response.body().begin() + pos);
        to_send_len = left_len < max_len ? left_len : max_len;
        size_t sent_len = send(_fd, &(_response.body()[pos]), to_send_len, 0);
        if (sent_len != to_send_len)
            throw std::runtime_error("send: failed");
        pos += sent_len;
        _response.body().set_pos(pos);
        max_len -= sent_len;
        if (sent_len == left_len && found_needle) {
            _response.set_state(http::Response::BODY);
            if (_cgi_handler.is_done() && pos >= _response.body().size()) {
                if (max_len >= 5) {
                    if (send(_fd, "0\r\n\r\n", 5, 0) != 5)
                        throw std::runtime_error("send: failed");
                    _response.set_state(http::Response::DONE);
                }
            }
        } else if (pos >= _response.body().size()) {
            eni.disable_event(_fd, EVFILT_WRITE);
            eni.delete_event(_fd, EVFILT_TIMER);
            return false;
        }
    }
    if (_response.state() == http::Response::BODY && max_len > 0) {
        switch (_response.body_type()) {
            case http::Response::BODY_BUFFER: {
                pos = _response.body().pos();
                left_len = _response.body().size() - pos;
                to_send_len = left_len < max_len ? left_len : max_len;
                size_t sent_len = send(_fd, &(_response.body()[pos]), to_send_len, 0);
                if (sent_len != to_send_len)
                    throw std::runtime_error("send: failed");
                pos += sent_len;
                _response.body().set_pos(pos);
                max_len -= sent_len;
                if (pos >= _response.body().size())
                    _response.set_state(http::Response::DONE);
                break;
            }
            case http::Response::BODY_CGI: {
                if (max_len < _max_pipe_size_str.size() + 4)  // \r\n\r\n
                    return true;
                size_t max_chunk_cont_len = max_len - _max_pipe_size_str.size() - 4;
                pos = _response.body().pos();
                left_len = _response.body().size() - pos;
                to_send_len = left_len < max_chunk_cont_len ? left_len : max_chunk_cont_len;
                if (to_send_len > 0) {
                    std::string chunk;
                    chunk.reserve(to_send_len + _max_pipe_size_str.size() + 4);
                    utils::num_to_str_hex(to_send_len, chunk);
                    chunk += "\r\n";
                    chunk.insert(chunk.end(), _response.body().begin() + pos,
                                 _response.body().begin() + pos + to_send_len);
                    chunk += "\r\n";
                    if (send(_fd, chunk.c_str(), chunk.size(), 0) != (ssize_t)chunk.size())
                        throw std::runtime_error("send: failed");
                    max_len -= chunk.size();
                    pos += to_send_len;
                    _response.body().set_pos(pos);
                }
                if (pos >= _response.body().size()) {
                    if (_cgi_handler.is_done()) {
                        if (max_len >= 5) {
                            if (send(_fd, "0\r\n\r\n", 5, 0) != 5)
                                throw std::runtime_error("send: failed");
                            _response.set_state(http::Response::DONE);
                        }
                    } else {
                        eni.disable_event(_fd, EVFILT_WRITE);
                        eni.delete_event(_fd, EVFILT_TIMER);
                        return false;
                    }
                }
                break;
            }
            case http::Response::BODY_FILE: {
                size_t to_send_len = _response.file_handler().read(max_len);
                if (to_send_len > 0) {
                    if (send(_fd, _response.file_handler().buf(), to_send_len, 0) !=
                        (ssize_t)to_send_len)
                        throw std::runtime_error("send: failed");
                }
                if (_response.file_handler().left_size() == 0) {
                    _response.set_state(http::Response::DONE);
                }
                break;
            }
            case http::Response::BODY_NONE:
                break;
        }
    }
    if (_response.state() == http::Response::DONE) {
        _is_response_done = true;
        _is_active = false;
        _request.init();
    }
    return true;
}

void Connection::destroy(EventNotificationInterface& eni) {
    close(_fd);
    _fd = -1;
    _cgi_handler.reset(eni);

#if PRINT_LEVEL > 0
    std::cout << utils::COLOR_YE << "[Closed]: " << utils::COLOR_NO
              << utils::addr_to_str(_client_addr) << std::endl;
#endif
}

bool operator==(const Connection& lhs, int fd) { return lhs.fd() == fd; }

bool operator==(int fd, const Connection& rhs) { return rhs.fd() == fd; }

bool operator==(const Connection& lhs, const Connection& rhs) { return lhs.fd() == rhs.fd(); }

}  // namespace core
