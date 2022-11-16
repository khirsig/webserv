#pragma once

#include <iostream>
#include <string>

namespace config {

enum TokenType { WHITESPACE, IDENTIFIER, OPERATOR, COMMENT, ESCAPE };

static const char *token_type_string[] = {"WHITESPACE", "IDENTIFIER", "OPERATOR", "COMMENT",
                                          "ESCAPE"};

class Token {
   public:
    enum TokenType type;
    std::string    text;
    std::size_t    line_number;

    void debug_print() const;
};

}  // namespace config