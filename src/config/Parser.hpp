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

    bool _is_config_valid(const std::string &file_path, const std::vector<Server> &v_server);
    void _inherit_client_max_body_size(std::vector<Server> &v_server);
    void _check_duplicate_listen(const std::string &file_path, const std::vector<Server> &v_server);
};

}  // namespace config
