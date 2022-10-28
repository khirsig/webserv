#include "Connection.hpp"

#include "../utils/num_to_str.hpp"
#include "Webserver.hpp"

namespace core {

const std::string Connection::_max_pipe_size_str = utils::num_to_str_hex(MAX_PIPE_SIZE);

Connection::Connection() {}

Connection::~Connection() {}

bool Connection::is_active() const { return _is_active; }

bool Connection::is_request_done() const { return _is_request_done; }

bool Connection::is_response_done() const { return _is_response_done; }
bool Connection::should_close() const { return _should_close; }

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

void Connection::parse_request(char* read_buf, size_t recved_len) {
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

void Connection::send_response(EventNotificationInterface& eni, size_t max_len) {
    size_t left_len;
    size_t to_send_len;
    size_t pos;
    if (_response.state() == http::Response::State::HEADER) {
        pos = _response.header().pos();
        left_len = _response.header().size() - pos;
        to_send_len = left_len < max_len ? left_len : max_len;
        size_t sent_len = send(_fd, &(_response.header()[pos]), to_send_len, 0);
        if (sent_len != to_send_len)
            throw std::runtime_error("send: failed");
        pos += sent_len;
        _response.header().set_pos(pos);
        max_len -= sent_len;
        if (pos >= _response.header().size())
            _response.set_state(http::Response::State::BODY);
    }
    if (_response.state() == http::Response::State::BODY && max_len > 0) {
        switch (_response.body_type()) {
            case http::Response::BodyType::BUFFER:
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
                    _response.set_state(http::Response::State::DONE);
                break;
            case http::Response::BodyType::CGI: {
                if (max_len < _max_pipe_size_str.size() + 4)  // \r\n\r\n
                    return;
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
                                 _response.body().begin() + to_send_len);
                    chunk += "\r\n";
                    if (send(_fd, chunk.c_str(), chunk.size(), 0) != chunk.size())
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
                            _response.set_state(http::Response::State::DONE);
                        }
                    } else {
                        eni.disable_event(_fd, EVFILT_WRITE);
                    }
                }
                break;
            }
            case http::Response::BodyType::FILE:
                size_t to_send_len = _response.file_handler().read(read_buf, read_buf_size);
                if (to_send_len > 0) {
                    if (send(_fd, read_buf, to_send_len, 0) != to_send_len)
                        throw std::runtime_error("send: failed");
                }
                if (_response.file_handler().left_size() == 0)
                    _response.set_state(http::Response::State::DONE);
                break;
            case http::Response::BodyType::NONE:
                break;
        }
    }
    // if (_response.state() == http::Response::State::DONE) {

    // }
}

}  // namespace core
