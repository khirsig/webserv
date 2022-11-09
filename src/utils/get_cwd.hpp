#pragma once

#include <string>

namespace utils {

std::string get_cwd();
std::string get_absolute_path(const std::string &path);

}  // namespace utils
