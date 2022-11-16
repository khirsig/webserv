#include "Token.hpp"

namespace config {

void Token::debug_print() const {
    std::cout << "Token(" << token_type_string[type] << ", \"" << text << "\", " << line_number
              << ")"
              << "\n";
}

}  // namespace config
