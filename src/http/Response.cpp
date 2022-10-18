#include "Response.hpp"

namespace http {

Response::Response(core::ByteBuffer& buf) : _buf(buf) {}

Response::~Response() {}

}  // namespace http