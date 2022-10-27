#include "Connection.hpp"
namespace core {

Connection::Connection() {}

Connection::~Connection() {}

bool Connection::is_active() const { return _is_active; }

bool Connection::is_request_done() const { return _is_request_done; }

void Connection::init(int fd, Address client_addr, Address socket_addr) {
    _fd = fd;
    _client_addr = client_addr;
    _socket_addr = socket_addr;
    _should_close = false;
    _is_active = false;
    _is_request_done = false;
    _request_error = 0;
    _request.init();
    _response.init();
}

void Connection::parse_request(char *read_buf, size_t recved_len) {
    try {
        _is_request_done = _request.parse(read_buf, recved_len);
    } catch (int error) {
        _request_error = error;
        _is_request_done = true;
    }
}

void Connection::build_response() {
    if (!_request_error)
        _response.build();
    else
        _response.build_error();
}

void Connection::send_response(size_t max_len) {
    size_t left_len;
    size_t to_send_len;
    if (_response.state() == http::Response::State::HEADER) {
        size_t &pos = _response.header().pos;
        left_len = _response.header().size() - pos;
        to_send_len = left_len < max_len ? left_len : max_len;
        size_t sent_len = send(_fd, &(_response.header()[pos]), to_send_len, 0);
        if (sent_len != to_send_len)
            throw std::runtime_error("send: failed");
        pos += sent_len;
        max_len -= sent_len;
    }
    if (_response.state() == http::Response::State::BODY && max_len > 0) {
    }
    if (_response.state() == http::Response::State::DONE) {
    }
}

}  // namespace core
