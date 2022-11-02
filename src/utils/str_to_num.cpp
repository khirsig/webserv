#include "str_to_num.hpp"

namespace utils {

bool str_to_num_dec(const std::string& str, size_t& num) {
    size_t tmp;
    num = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9')
            return false;
        tmp = num;
        num = num * 10 + (str[i] - '0');
        if (num < tmp)
            return false;
    }
    return true;
}

}  // namespace utils
