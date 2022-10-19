#pragma once

#include <map>

#include "../core/ByteBuffer.hpp"

namespace http {

class ErrorPages {
   private:
   public:
    std::map<int, core::ByteBuffer> pages;
    void                            init();
};

//
// GLOBAL
//

extern ErrorPages g_error_pages;

}  // namespace http
