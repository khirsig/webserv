#include "Address.hpp"

namespace core {

bool Address::operator==(const Address &rhs) { return addr == rhs.addr && port == rhs.port; }

}  // namespace core
