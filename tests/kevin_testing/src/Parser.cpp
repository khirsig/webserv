/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 10:35:16 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/15 10:51:09 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

namespace config {

void Parser::parse(const std::string &file_path, std::vector<Server> &v_server) {
    Tokenizer          tokenizer;
    std::string        file_content = _file_to_string(file_path);
    std::vector<Token> v_token;

    tokenizer.parse(v_token, file_content);

    Interpreter interpreter(file_path);
    interpreter.parse(v_token, v_server);
}

std::string Parser::_file_to_string(std::string file_path) {
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    file.open(file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file \"" << file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    return (file_content);
}

}  // namespace config
