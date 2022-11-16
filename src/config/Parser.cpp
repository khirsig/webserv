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

    _inherit_client_max_body_size(v_server);

    if (!_is_config_valid(file_path, v_server))
        exit(EXIT_FAILURE);
    _check_duplicate_listen(file_path, v_server);
    _check_duplicate_cgi_pass(file_path, v_server);
}

std::string Parser::_file_to_string(std::string file_path) {
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    file.open(file_path);
    if (!file.is_open()) {
        utils::print_timestamp(std::cerr);
        std::cerr << " " << strerror(errno) << ": \"" << file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    return (file_content);
}

bool Parser::_is_config_valid(const std::string &file_path, const std::vector<Server> &v_server) {
    if (v_server.empty()) {
        utils::print_timestamp(std::cerr);
        std::cerr << " at least one server required in config in " << file_path << std::endl;
        return (false);
    }
    for (std::vector<Server>::const_iterator it = v_server.begin(); it != v_server.end(); ++it) {
        if (it->v_listen.empty()) {
            utils::print_timestamp(std::cerr);
            std::cerr << " at least one listen per server required in " << file_path << std::endl;
            return (false);
        }
        if (it->v_location.empty()) {
            utils::print_timestamp(std::cerr);
            std::cerr << " at least one location per server required in " << file_path << std::endl;
            return (false);
        }
        for (std::vector<Location>::const_iterator it2 = it->v_location.begin();
             it2 != it->v_location.end(); ++it2) {
            if (it2->root.empty()) {
                utils::print_timestamp(std::cerr);
                std::cerr << " at least one root per location required in " << file_path
                          << std::endl;
                return (false);
            }
        }
    }
    return (true);
}

void Parser::_inherit_client_max_body_size(std::vector<Server> &v_server) {
    for (std::vector<Server>::iterator it = v_server.begin(); it != v_server.end(); ++it) {
        if (it->client_max_body_size == SIZE_MAX)
            it->client_max_body_size = CLIENT_MAX_BODY_SIZE;
        for (std::vector<Location>::iterator it2 = it->v_location.begin();
             it2 != it->v_location.end(); ++it2) {
            if (it2->client_max_body_size == SIZE_MAX)
                it2->client_max_body_size = it->client_max_body_size;
        }
    }
}

void Parser::_check_duplicate_listen(const std::string         &file_path,
                                     const std::vector<Server> &v_server) {
    for (std::vector<Server>::const_iterator it = v_server.begin(); it != v_server.end(); ++it) {
        for (std::vector<core::Address>::const_iterator it2 = it->v_listen.begin();
             it2 != it->v_listen.end(); ++it2) {
            for (std::vector<core::Address>::const_iterator it3 = it->v_listen.begin();
                 it3 != it->v_listen.end(); ++it3) {
                if (it2 != it3 && it2->port == it3->port) {
                    if (it2->addr == it3->addr || it2->addr == INADDR_ANY ||
                        it3->addr == INADDR_ANY)
                        utils::print_timestamp(std::cerr);
                    std::cerr << " no duplicate listens inside servers allowed in " << file_path
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    for (std::vector<Server>::const_iterator it = v_server.begin(); it != v_server.end(); ++it) {
        for (std::vector<Server>::const_iterator it2 = v_server.begin(); it2 != v_server.end();
             ++it2) {
            if (it != it2) {
                for (std::vector<core::Address>::const_iterator it3 = it->v_listen.begin();
                     it3 != it->v_listen.end(); ++it3) {
                    for (std::vector<core::Address>::const_iterator it4 = it2->v_listen.begin();
                         it4 != it2->v_listen.end(); ++it4) {
                        if (it3->port == it4->port) {
                            if (it3->addr == it4->addr || it3->addr == INADDR_ANY ||
                                it4->addr == INADDR_ANY) {
                                for (std::vector<std::string>::const_iterator it5 =
                                         it->v_server_name.begin();
                                     it5 != it->v_server_name.end(); ++it5) {
                                    for (std::vector<std::string>::const_iterator it6 =
                                             it2->v_server_name.begin();
                                         it6 != it2->v_server_name.end(); ++it6) {
                                        if (*it5 == *it6) {
                                            utils::print_timestamp(std::cerr);
                                            std::cerr << " no duplicate server (same listen and "
                                                         "server_name) allowed in "
                                                      << file_path << std::endl;
                                            exit(EXIT_FAILURE);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Parser::_check_duplicate_cgi_pass(const std::string         &file_path,
                                       const std::vector<Server> &v_server) {
    for (std::vector<Server>::const_iterator it = v_server.begin(); it != v_server.end(); ++it) {
        for (std::vector<Location>::const_iterator it2 = it->v_location.begin();
             it2 != it->v_location.end(); ++it2) {
            for (std::vector<CgiPass>::const_iterator it3 = it2->v_cgi_pass.begin();
                 it3 != it2->v_cgi_pass.end(); ++it3) {
                for (std::vector<CgiPass>::const_iterator it4 = it2->v_cgi_pass.begin();
                     it4 != it2->v_cgi_pass.end(); ++it4) {
                    if (it3 != it4 && it3->type == it4->type) {
                        utils::print_timestamp(std::cerr);
                        std::cerr << " no duplicate types in cgi_pass allowed in " << file_path
                                  << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
}

}  // namespace config
