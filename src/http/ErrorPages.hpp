#pragma once

#include <map>

#include "../core/ByteBuffer.hpp"

namespace http {

class ErrorPages {
   private:
   public:
    std::map<int, core::ByteBuffer> pages;
    void                            init();
    void insert_page(int error_code, const core::ByteBuffer& body, const std::string& content_type);
};

//
// GLOBAL
//

extern ErrorPages g_error_pages;

}  // namespace http
