#include "get_cwd.hpp"

#include <limits.h>
#include <unistd.h>

#include <string>

namespace utils {

std::string get_cwd() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string get_absolute_path(const std::string &path) {
    if (path.size() == 0 || path[0] == '/')
        return path;
    return (get_cwd() + "/" + path);
}

}  // namespace utils
