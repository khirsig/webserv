#include "Connection.hpp"
namespace core {

Connection::Conection() {}

Connection::~Connection() {}

bool Connection::is_active() const { return _is_active; }

bool Connection::is_request_done() const { return _is_request_done; }

void Connection::init(int fd, Address client_addr, Address socket_addr) {
    _client_addr = client_addr;
    _socket_addr = socket_addr;
    _should_close = false;
    _is_active = false;
    _is_request_done = false;
    _request.init();
    _response.init();
}

void Connection::parse_request(char *read_buf, ssize_t recved_len) {
    _is_request_done = _request.parse(read_buf, recved_len);
    if (_is_request_done) {
        _response.build()
    }
}

}  // namespace core
