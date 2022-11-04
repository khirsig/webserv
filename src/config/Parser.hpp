#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "Interpreter.hpp"
#include "Tokenizer.hpp"

namespace config {

class Parser {
   public:
    void parse(const std::string &file_path, std::vector<Server> &v_server);

   private:
    std::string _file_to_string(std::string file_path);
};

}  // namespace config
