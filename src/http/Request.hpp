#pragma once

#include <map>
#include <string>

#include "../core/ByteBuffer.hpp"

namespace http {

enum Method { GET, POST, DELETE, HEAD };

class Request {
   private:
    Method                             _method;
    std::string                        _path_encoded;
    std::string                        _path_decoded;
    std::string                        _query_string;
    std::string                        _host_encoded;
    std::string                        _host_decoded;
    std::map<std::string, std::string> _m_header;
    core::ByteBuffer                   _body;

   public:
};

}  // namespace http