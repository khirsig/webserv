#include "num_to_str.hpp"

namespace utils {

void num_to_str_hex(size_t num, std::string &str) {
    const char base[] = "0123456789ABCDEF";
    while (num) {
        str += base[num % 16];
        num /= 16;
    }
}

std::string num_to_str_hex(size_t num) {
    std::string str;
    const char  base[] = "0123456789ABCDEF";
    while (num) {
        str += base[num % 16];
        num /= 16;
    }
    return str;
}

void num_to_str_dec(size_t num, std::string &str) {
    while (num) {
        str += num % 10 + '0';
        num /= 10;
    }
}

std::string num_to_str_dec(size_t num) {
    std::string str;
    while (num) {
        str += num % 10 + '0';
        num /= 10;
    }
    return str;
}

}  // namespace utils
