#pragma once

#include <string>

#include "../core/ByteBuffer.hpp"

namespace http {

typedef struct s_error_page {
    std::string      content_type;
    core::ByteBuffer content;
} error_page_t;

}  // namespace http
