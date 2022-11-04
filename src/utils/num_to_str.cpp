#include "num_to_str.hpp"

namespace utils {

void num_to_str_hex(size_t num, std::string &str) {
    str.clear();
    if (num == 0) {
        str.push_back('0');
        return;
    }
    while (num) {
        size_t digit = num % 16;
        if (digit < 10)
            str.push_back('0' + digit);
        else
            str.push_back('a' + digit - 10);
        num /= 16;
    }
    std::reverse(str.begin(), str.end());
}

std::string num_to_str_hex(size_t num) {
    std::string str;
    if (num == 0) {
        str.push_back('0');
        return str;
    }
    while (num) {
        size_t digit = num % 16;
        if (digit < 10)
            str.push_back('0' + digit);
        else
            str.push_back('a' + digit - 10);
        num /= 16;
    }
    std::reverse(str.begin(), str.end());
    return str;
}

void num_to_str_dec(size_t num, std::string &str) {
    str.clear();
    if (num == 0) {
        str.push_back('0');
        return;
    }
    while (num) {
        str.push_back('0' + (num % 10));
        num /= 10;
    }
    std::reverse(str.begin(), str.end());
}

std::string num_to_str_dec(size_t num) {
    std::string str;
    if (num == 0) {
        str.push_back('0');
        return str;
    }
    while (num) {
        str.push_back('0' + (num % 10));
        num /= 10;
    }
    std::reverse(str.begin(), str.end());
    return str;
}

}  // namespace utils
