#pragma once

#include <map>

#include "../core/ByteBuffer.hpp"

namespace http {

class StatusCodes {
   private:
   public:
    std::map<int, core::ByteBuffer> codes;

    void init();

    // iterator find(int error);
    // core::ByteBuffer& operator[](int error) { return m_status_code[error]; }
    // iterator       begin();
    // const_iterator begin() const;
    // iterator       end();
    // const_iterator end() const;
};

//
// GLOBAL
//

extern StatusCodes g_status_codes;

}  // namespace http
