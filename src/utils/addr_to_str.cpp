#include "addr_to_str.hpp"

#include <arpa/inet.h>

#include <string>

#include "../core/Address.hpp"
#include "num_to_str.hpp"

namespace utils {

std::string addr_to_str(const core::Address& addr) {
    struct in_addr in_addr;
    in_addr.s_addr = addr.addr;
    std::string str;
    str += inet_ntoa(in_addr);
    str += ":";
    str += num_to_str_dec(addr.port);
    return str;
}

}  // namespace utils
