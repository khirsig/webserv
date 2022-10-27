#pragma once

#include <arpa/inet.h>

namespace core {

struct Address {
    in_addr_t addr;
    in_port_t port;

    bool operator==(const Address &rhs);
};

}