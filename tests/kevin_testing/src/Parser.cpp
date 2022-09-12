/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/12 09:48:20 by khirsig          ###   ########.fr       */
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
                _parse_identifier(v_token, it, new_server.v_listen);
            } else if (it->text == "server_name") {
                _parse_identifier(v_token, it, new_server.v_server_name);
            } else if (it->text == "error_page") {
                _parse_identifier(v_token, it, new_server.v_error_page);
            } else if (it->text == "client_max_body_size") {
                _parse_identifier(it, new_server.client_max_body_size);
            } else if (it->text == "location") {
                Location temp;
                temp = _parse_location(v_token, it);
                new_server.v_location.push_back(temp);
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

Location Parser::_parse_location(const std::vector<Token>           &v_token,
                                 std::vector<Token>::const_iterator &it) {
    ++it;
    Location new_location;
    if (it->type == IDENTIFIER) {
        _parse_location(v_token, it, new_location.v_path);
    } else {
        std::cout << "No identifier for location in " << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }

    if (it->text == "{" && it->type == OPERATOR) {
        ++it;
        for (; it != v_token.end() && it->text != "}"; ++it) {
            _last_directive = &(it->text);
            if (*_last_directive == "accepted_methods") {
                _parse_identifier(v_token, it, new_location.v_accepted_method);
            } else if (*_last_directive == "redirect") {
                _parse_identifier(v_token, it, new_location.v_redirect);
            } else if (*_last_directive == "root") {
                _parse_identifier(it, new_location.root);
            } else if (*_last_directive == "index") {
                _parse_identifier(v_token, it, new_location.v_index);
            } else if (*_last_directive == "location") {
                Location temp;
                temp = _parse_location(v_token, it);
                new_location.v_location.push_back(temp);
            } else if (*_last_directive == "cgi_pass") {
                _parse_identifier(v_token, it, new_location.v_cgi_pass);
            } else if (*_last_directive == "directory_listing") {
                _parse_bool(it, new_location.directory_listing);
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
    return (new_location);
}

void Parser::_parse_identifier(const std::vector<Token>           &v_token,
                               std::vector<Token>::const_iterator &it,
                               std::vector<std::string>           &v_identifier) {
    ++it;
    for (; it != v_token.end() && it->text != ";"; ++it) {
        std::string str(it->text);
        v_identifier.push_back(str);
        str.erase();
    }
    if (it->text != ";") {
        std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in "
                  << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
}

void Parser::_parse_identifier(std::vector<Token>::const_iterator &it, std::string &identifier) {
    ++it;
    identifier = it->text;
    ++it;
    if (it->text != ";") {
        std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in "
                  << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
}

void Parser::_parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it,
                             std::vector<LocationPath>          &v_path) {
    LocationPath new_path;

    if (it->text.size() <= 0) {
        exit(EXIT_FAILURE);
    }
    if (it->text[0] == '*') {
        new_path.wildcard = PREFIX;
        new_path.str = it->text;
        new_path.str.erase(0, 1);
    } else if (it->text[it->text.size() - 1] == '*') {
        new_path.wildcard = POSTFIX;
        new_path.str = it->text;
        new_path.str.erase(it->text.size() - 1, 1);
    } else {
        new_path.wildcard = NONE;
        new_path.str = it->text;
    }
    ++it;

    if (it->text == "(" && it->type == OPERATOR) {
        ++it;
        for (; it != v_token.end() && it->text != ")"; ++it) {
            LocationPath multi_path(new_path);
            multi_path.str += it->text;
            v_path.push_back(multi_path);
        }
        if (it == v_token.end()) {
            std::cerr << "Missing \")\" in " << _path << ":" << (it - 1)->line_number + 1 << "\n";
            exit(EXIT_FAILURE);
        }
        ++it;
    } else {
        v_path.push_back(new_path);
    }
}

void Parser::_parse_bool(std::vector<Token>::const_iterator &it, bool &identifier) {
    ++it;
    if (it->text == "on") {
        identifier = true;
    } else if (it->text == "off") {
        identifier = false;
    } else {
        std::cerr << "\"" << *_last_directive << "\" wrong input for bool in " << _path << ":"
                  << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
    ++it;
    if (it->text != ";") {
        std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in "
                  << _path << ":" << it->line_number << "\n";
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