#pragma once

#include <string>

namespace http {

struct s_mime_types {
    const char* ending;
    const char* content_type;
};

const char* mime_type(const std::string& file);

}  // namespace http
