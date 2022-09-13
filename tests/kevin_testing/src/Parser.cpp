/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 14:01:28 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

namespace ft {

void Parser::parse(const std::vector<Token> &v_token, std::vector<Server> &v_server) {
    for (std::vector<Token>::const_iterator it = v_token.begin(); it != v_token.end(); ++it) {
        if (it->text == "server" && it->type == IDENTIFIER) {
            Server new_server = _parse_server(v_token, it);
            v_server.push_back(new_server);
        }
    }
}

Server Parser::_parse_server(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it) {
    ++it;
    Server new_server;
    if (it->text == "{" && it->type == OPERATOR) {
        ++it;
        for (; it != v_token.end() && it->text != "}"; ++it) {
            if (it->text == "listen") {
                _parse_identifier(v_token, it, new_server.get_v_listen(), "listen");
            } else if (it->text == "server_name") {
                _parse_identifier(v_token, it, new_server.get_v_server_name(), "server_name");
            } else if (it->text == "error_page") {
                _parse_identifier(v_token, it, new_server.get_v_error_page(), "error_page");
            } else if (it->text == "client_max_body_size") {
                _parse_identifier(it, new_server.get_client_max_body_size(),
                                  "client_max_body_size");
            } else {
                std::cerr << "\"" << it->text << "\" directive is not allowed here in " << _path
                          << ":" << it->line_number << "\n";
                exit(EXIT_FAILURE);
            }
        }
        if (it == v_token.end() || it->text != "}") {
            std::cout << "unexpected end of file, expecting \"}\" in " << _path << ":"
                      << (it - 1)->line_number + 1 << "\n";
            exit(EXIT_FAILURE);
        }
    } else {
        _exit_error(*it, "{", OPERATOR);
    }
    return (new_server);
}

void Parser::_parse_identifier(const std::vector<Token>           &v_token,
                               std::vector<Token>::const_iterator &it,
                               std::vector<std::string> &v_identifier, std::string err_code) {
    ++it;
    for (; it != v_token.end() && it->text != ";"; ++it) {
        std::string str(it->text);
        v_identifier.push_back(str);
        str.erase();
    }
    if (it->text != ";") {
        std::cerr << "directive \"" << err_code << "\" is not terminated by \";\" in " << _path
                  << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
}

void Parser::_parse_identifier(std::vector<Token>::const_iterator &it, std::string &identifier,
                               std::string err_code) {
    ++it;
    identifier = it->text;
    ++it;
    if (it->text != ";") {
        std::cerr << "directive \"" << err_code << "\" is not terminated by \";\" in " << _path
                  << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
}

void Parser::_exit_error(const Token &false_token, const std::string expected_text,
                         const std::size_t expected_type) {
    std::cerr << "Error: Expected: " << token_type_string[expected_type] << " + \"" << expected_text
              << "\" Actual: " << token_type_string[false_token.type] << " + \"" << false_token.text
              << "\"." << std::endl;
    exit(EXIT_FAILURE);
}

}  // namespace ft