#include "Response.hpp"

namespace http {

Response::Response() : _body_type(NONE), _state(HEADER) {}

Response::~Response() {}

Response::State Response::state() const { return _state; }

Response::BodyType Response::body_type() const { return _body_type; }

core::ByteBuffer &Response::header() { return _header; }

core::ByteBuffer &Response::body() { return _body; }

void Response::set_state(Response::State new_state) { _state = new_state; }

core::FileHandler &Response::file_handler() { return _file_handler; }

}  // namespace http
