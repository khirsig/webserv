#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

namespace config {

class Tokenizer {
   public:
    void parse(std::vector<Token> &v_token, const std::string &input_file);

   private:
    void _end_token(std::vector<Token> &v_token, Token &current_token);
};

}  // namespace config
