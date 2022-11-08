#include "Parser.hpp"

#include "../utils/timestamp.hpp"

namespace config {

void Parser::parse(const std::string &file_path, std::vector<Server> &v_server) {
    Tokenizer          tokenizer;
    std::string        file_content = _file_to_string(file_path);
    std::vector<Token> v_token;

    tokenizer.parse(v_token, file_content);

    Interpreter interpreter(file_path);
    interpreter.parse(v_token, v_server);

    if (!_is_config_valid(file_path, v_server))
        exit(EXIT_FAILURE);
}

std::string Parser::_file_to_string(std::string file_path) {
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    file.open(file_path);
    if (!file.is_open()) {
        std::cerr << strerror(errno) << ": \"" << file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    return (file_content);
}

// Function to check if config is valid
// 1. v_server must not be empty
// 2. In each server, v_listen must not be empty
// 3. In each server, v_location must not be empty
// 4. In each location, root must not be empty
bool Parser::_is_config_valid(const std::string &file_path, const std::vector<Server> &v_server) {
    if (v_server.empty()) {
        utils::print_timestamp(std::cerr);
        std::cerr << " atleast one server required in config in " << file_path << std::endl;
        return (false);
    }
    for (std::vector<Server>::const_iterator it = v_server.begin(); it != v_server.end(); ++it) {
        if (it->v_listen.empty()) {
            utils::print_timestamp(std::cerr);
            std::cerr << " atleast one listen per server required in " << file_path << std::endl;
            return (false);
        }
        if (it->v_location.empty()) {
            utils::print_timestamp(std::cerr);
            std::cerr << " atleast one location per server required in " << file_path << std::endl;
            return (false);
        }
        for (std::vector<Location>::const_iterator it2 = it->v_location.begin();
             it2 != it->v_location.end(); ++it2) {
            if (it2->root.empty()) {
                utils::print_timestamp(std::cerr);
                std::cerr << " atleast one root per location required in " << file_path
                          << std::endl;
                return (false);
            }
        }
    }
    return (true);
}

}  // namespace config
