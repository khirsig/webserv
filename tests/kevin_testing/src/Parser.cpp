/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/14 14:58:48 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

namespace config {

void Parser::parse(const std::vector<Token> &v_token, std::vector<Server> &v_server) {
    for (std::vector<Token>::const_iterator it = v_token.begin(); it != v_token.end(); ++it) {
        if (it->text == "server" && it->type == IDENTIFIER) {
            Server new_server = _parse_server(v_token, it);
            v_server.push_back(new_server);
        } else {
            _invalid_directive(it);
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
            _last_directive = &(it->text);
            if (*_last_directive == "listen") {
                _parse_string(v_token, it, new_server.v_listen);
            } else if (*_last_directive == "server_name") {
                _parse_string(v_token, it, new_server.v_server_name);
            } else if (*_last_directive == "error_page") {
                _parse_string(v_token, it, new_server.v_error_page);
            } else if (*_last_directive == "client_max_body_size") {
                _parse_bytes(it, new_server.client_max_body_size);
            } else if (*_last_directive == "location") {
                Location temp;
                temp = _parse_location(v_token, it);
                new_server.v_location.push_back(temp);
            } else {
                _invalid_directive(it);
            }
        }
        if (it == v_token.end() || it->text != "}") {
            _unexpected_file_ending(it);
        }
    } else {
        _missing_opening(it, '{');
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
                _parse_string(v_token, it, new_location.v_accepted_method);
            } else if (*_last_directive == "redirect") {
                _parse_string(v_token, it, new_location.v_redirect);
            } else if (*_last_directive == "root") {
                _parse_string(it, new_location.root);
            } else if (*_last_directive == "index") {
                _parse_string(v_token, it, new_location.v_index);
            } else if (*_last_directive == "location") {
                Location temp;
                temp = _parse_location(v_token, it);
                new_location.v_location.push_back(temp);
            } else if (*_last_directive == "cgi_pass") {
                _parse_string(v_token, it, new_location.v_cgi_pass);
            } else if (*_last_directive == "directory_listing") {
                _parse_bool(it, new_location.directory_listing);
            } else {
                _invalid_directive(it);
            }
        }
        if (it == v_token.end() || it->text != "}") {
            _unexpected_file_ending(it);
        }
    } else {
        _missing_opening(it, '{');
    }
    return (new_location);
}

void Parser::_parse_string(const std::vector<Token>           &v_token,
                           std::vector<Token>::const_iterator &it,
                           std::vector<std::string>           &v_identifier) {
    ++it;
    for (; it != v_token.end(); ++it) {
        if (it->type == OPERATOR) {
            if (it->text == ";")
                break;
            else {
                _none_terminated_directive(it);
            }
        }
        std::string str(it->text);
        v_identifier.push_back(str);
        str.erase();
    }
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Parser::_parse_string(std::vector<Token>::const_iterator &it, std::string &identifier) {
    ++it;
    identifier = it->text;
    ++it;
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Parser::_parse_bytes(std::vector<Token>::const_iterator &it, std::uint64_t &identifier) {
    ++it;
    std::string   num = it->text;
    std::uint64_t multiplier;
    char          byte_size = num[it->text.size() - 1];

    if (byte_size == 'G' || byte_size == 'g') {
        multiplier = 1000000000;
        num.erase(num.size() - 1, 1);
    } else if (byte_size == 'M' || byte_size == 'm') {
        multiplier = 1000000;
        num.erase(num.size() - 1, 1);
    } else if (byte_size == 'K' || byte_size == 'k') {
        multiplier = 1000;
        num.erase(num.size() - 1, 1);
    } else if (byte_size == 'B' || byte_size == 'b') {
        multiplier = 1;
        num.erase(num.size() - 1, 1);
    } else if (byte_size >= '0' && byte_size <= '9') {
        multiplier = 1;
    } else {
        std::cerr << "size identifier is wrong\n";
        exit(EXIT_FAILURE);
    }

    if (num.size() > 3) {
        std::cerr << "numeric amount too high. Maximum 3 digits allowed. Use 'B', 'K', 'M', 'G' "
                     "instead\n";
        exit(EXIT_FAILURE);
    }

    if (num.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "numeric characters expected but has other type.\n";
        exit(EXIT_FAILURE);
    }

    char *p_end;
    identifier = strtol(num.c_str(), &p_end, 10) * multiplier;

    ++it;
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Parser::_parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it,
                             std::vector<LocationPath>          &v_path) {
    LocationPath new_path;

    if (it->text.size() <= 0) {
        _invalid_directive_argument_amount(it);
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
            if (it->text != ")" && it->type == OPERATOR) {
                std::cerr << "multiple location path "
                          << "is not terminated by \")\" in " << _path << ":" << it->line_number
                          << "\n";
                exit(EXIT_FAILURE);
            }
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
        _invalid_bool_argument(it);
    }
    ++it;
    if (it->text != ";") {
        _invalid_directive(it);
    }
}

std::string Parser::_timestamp() const {
    std::time_t t = time(NULL);
    struct tm  *time_master = localtime(&t);
    std::string ret;

    std::cerr << time_master->tm_year + 1900 << "/" << time_master->tm_mon + 1 << "/"
              << time_master->tm_mday << " " << time_master->tm_hour << ":" << time_master->tm_min
              << ":" << time_master->tm_sec << " ";

    return ret;
}

void Parser::_invalid_directive(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "\"" << *_last_directive << "\" directive is not allowed here in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Parser::_unexpected_file_ending(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "unexpected end of file, expecting \"}\" in " << _path << ":"
              << (it - 1)->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Parser::_none_terminated_directive(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in " << _path
              << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Parser::_invalid_bool_argument(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "invalid value \"" << it->text << "\" in \"" << *_last_directive
              << "\" directive, it must be \"on\" or \"off\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Parser::_invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "invalid number of arguments in \"" << *_last_directive << "\" directive in path"
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Parser::_missing_opening(std::vector<Token>::const_iterator &it, const char &op) const {
    _timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" has no opening \"" << op << "\" in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

}  // namespace config