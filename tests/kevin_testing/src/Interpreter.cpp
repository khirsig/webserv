/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/19 11:48:31 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interpreter.hpp"

namespace config {

void Interpreter::parse(const std::vector<Token> &v_token, std::vector<Server> &v_server) {
    for (std::vector<Token>::const_iterator it = v_token.begin(); it != v_token.end(); ++it) {
        if (it->text == "server" && it->type == IDENTIFIER) {
            Server new_server = _parse_server(v_token, it);
            v_server.push_back(new_server);
        } else {
            _invalid_directive(it);
        }
    }
}

Server Interpreter::_parse_server(const std::vector<Token>           &v_token,
                                  std::vector<Token>::const_iterator &it) {
    ++it;
    Server new_server;
    if (it->text == "{" && it->type == OPERATOR) {
        ++it;
        for (; it != v_token.end() && it->text != "}"; ++it) {
            _last_directive = &(it->text);
            if (*_last_directive == "listen") {
                Listen new_listen;
                if (_parse_listen(v_token, it, new_listen))
                    new_server.v_listen.insert(new_server.v_listen.begin(), new_listen);
                else
                    new_server.v_listen.push_back(new_listen);
            } else if (*_last_directive == "server_name") {
                _parse_string(v_token, it, new_server.v_server_name);
            } else if (*_last_directive == "error_page") {
                ErrorPage new_error_page;
                _parse_error_page(v_token, it, new_error_page);
                new_server.v_error_page.push_back(new_error_page);
            } else if (*_last_directive == "client_max_body_size") {
                _parse_bytes(it, new_server.client_max_body_size);
            } else if (*_last_directive == "location") {
                Location new_location;
                new_location = _parse_location(v_token, it);
                new_server.v_location.push_back(new_location);
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

Location Interpreter::_parse_location(const std::vector<Token>           &v_token,
                                      std::vector<Token>::const_iterator &it) {
    ++it;
    Location new_location;
    if (it->type == IDENTIFIER || it->text == "*" || it->text == "(") {
        _parse_location_path(v_token, it, new_location.v_path);
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
                Redirect new_redirect;
                _parse_redirect(v_token, it, new_redirect);
                new_location.v_redirect.push_back(new_redirect);
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

void Interpreter::_parse_string(const std::vector<Token>           &v_token,
                                std::vector<Token>::const_iterator &it,
                                std::vector<std::string>           &v_identifier) {
    ++it;
    for (; it != v_token.end(); ++it) {
        if (it->type == OPERATOR) {
            if (it->text == ";")
                break;
            else {
                _unexpected_operator(it);
            }
        }
        std::string str(it->text);
        if (*_last_directive == "accepted_methods") {
            if (!(str == "GET" || str == "POST" || str == "DELETE" || str == "HEAD")) {
                std::cerr << "\"" << str << "\" is not allowed as \"accepted_method\" in " << _path
                          << ":" << it->line_number << "\n";
                exit(EXIT_FAILURE);
            }
        }
        v_identifier.push_back(str);
        str.erase();
    }
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Interpreter::_parse_string(std::vector<Token>::const_iterator &it, std::string &identifier) {
    ++it;
    identifier = it->text;
    ++it;
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

bool Interpreter::_parse_listen(const std::vector<Token>           &v_token,
                                std::vector<Token>::const_iterator &it, Listen &identifier) {
    ++it;
    bool default_server = false;

    std::size_t seperator_index = it->text.find_first_of(':');
    if (seperator_index != it->text.find_last_of(':')) {
        std::cerr << "multiple \":\" operator used in " << _path << ":" << it->line_number << "\n";
    } else if (seperator_index == std::string::npos) {
        if (it->text.find_first_of('.') == std::string::npos) {
            _parse_port(it, it->text, identifier.port);
            identifier.addr = INADDR_ANY;
        } else {
            identifier.port = htons(80);
            identifier.addr = inet_addr(it->text.c_str());
        }
    } else {
        std::string addr_str = it->text.substr(0, seperator_index);
        std::string port_str =
            it->text.substr(seperator_index + 1, it->text.size() - seperator_index);

        identifier.addr = inet_addr(addr_str.c_str());
        _parse_port(it, port_str, identifier.port);
    }
    ++it;

    for (; it != v_token.end() && it->text != ";"; ++it) {
        if (it->text == "default_server") {
            default_server = true;
        } else {
            std::cerr << "invalid parameter \"" << it->text << "\" in " << _path << ":"
                      << it->line_number << "\n";
            exit(EXIT_FAILURE);
        }
    }

    if (it == v_token.end())
        _unexpected_file_ending(it);
    if (it->text != ";")
        _none_terminated_directive(it);

    return default_server;
}

void Interpreter::_parse_port(std::vector<Token>::const_iterator &it, const std::string &str,
                              in_port_t &port) {
    if (str.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "invalid port in \"" << str << "\" of the \"" << *_last_directive
                  << "\" directive in " << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
    int i;
    std::istringstream(str) >> i;
    port = htons(i);
}

void Interpreter::_parse_error_page(const std::vector<Token>           &v_token,
                                    std::vector<Token>::const_iterator &it, ErrorPage &identifier) {
    ++it;
    for (; it != v_token.end() && it->text != ";"; ++it) {
        if (it->text.find_first_not_of("0123456789") == std::string::npos) {
            std::uint32_t new_code;
            std::stringstream(it->text) >> new_code;
            identifier.v_code.push_back(new_code);
        } else {
            break;
        }
    }
    if (it == v_token.end() || it->text == ";" || identifier.v_code.size() <= 0) {
        if (it == v_token.end())
            _unexpected_file_ending(it);
        else
            _invalid_directive_argument_amount(it);
    }

    identifier.path = it->text;
    ++it;
    if (it == v_token.end())
        _unexpected_file_ending(it);
    if (it->text != ";")
        _none_terminated_directive(it);
}

void Interpreter::_parse_redirect(const std::vector<Token>           &v_token,
                                  std::vector<Token>::const_iterator &it, Redirect &identifier) {
    ++it;
    std::vector<Token>::const_iterator iter = it;
    std::uint32_t                      count = 0;
    for (; iter != v_token.end() && iter->text != ";"; ++iter) {
        if (iter->type == OPERATOR)
            _unexpected_operator(iter);
        count++;
    }
    if (count != 3)
        _invalid_directive_argument_amount(iter);

    if (it->text.find_first_not_of("0123456789") == std::string::npos) {
        std::stringstream(it->text) >> identifier.status_code;
    } else {
        _timestamp();
        std::cerr << "invalid status code for \"" << *_last_directive << "\" in " << _path << ":"
                  << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
    ++it;
    if (it == v_token.end())
        _unexpected_file_ending(it);
    identifier.origin = it->text;
    ++it;
    if (it == v_token.end())
        _unexpected_file_ending(it);
    identifier.direction = it->text;
    ++it;
    if (it == v_token.end())
        _unexpected_file_ending(it);
    if (it->text != ";")
        _none_terminated_directive(it);
}

void Interpreter::_parse_bytes(std::vector<Token>::const_iterator &it, std::uint64_t &identifier) {
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

void Interpreter::_parse_location_path(const std::vector<Token>           &v_token,
                                       std::vector<Token>::const_iterator &it,
                                       std::vector<LocationPath>          &v_path) {
    std::vector<LocationPath> v_path_temp;
    LocationPath              new_path;

    if (it->text.size() <= 0) {
        _invalid_directive_argument_amount(it);
    }
    if (it->text == "*" && it->type == OPERATOR) {
        new_path.wildcard = PREFIX;
        ++it;
    } else {
        new_path.wildcard = NONE;
    }

    for (; it != v_token.end() && it->text != "{"; ++it) {
        if (it->text == "(" && it->type == OPERATOR) {
            ++it;
            for (; it != v_token.end() && it->text != ")"; ++it) {
                if (it->text == "|" && it->type == OPERATOR) {
                    ++it;
                    if (it->text == "|" && it->type == OPERATOR) {
                        std::cerr << "multiple operator of type \"|\" in " << _path << ":"
                                  << it->line_number << "\n";
                        exit(EXIT_FAILURE);
                    }
                    if (it->text == ")" && it->type == OPERATOR) {
                        std::cerr << "missing identifier after operator of type \"|\" in " << _path
                                  << ":" << it->line_number << "\n";
                        exit(EXIT_FAILURE);
                    }
                }
                if (it->text != ")" && it->type == OPERATOR) {
                    std::cerr << "multiple location path "
                              << "is not terminated by \")\" in " << _path << ":" << it->line_number
                              << "\n";
                    exit(EXIT_FAILURE);
                }
                LocationPath multi_path(new_path);
                multi_path.str += it->text;
                v_path_temp.push_back(multi_path);
            }
            if (it == v_token.end()) {
                std::cerr << "missing \")\" in " << _path << ":" << (it - 1)->line_number + 1
                          << "\n";
                exit(EXIT_FAILURE);
            }
            if (new_path.str.size() > 0 && (it + 1)->text != "{") {
                std::cerr << "only post- or prefix regex wildcard allowed in " << _path << ":"
                          << it->line_number << "\n";
                exit(EXIT_FAILURE);
            }
        } else if (it->type == IDENTIFIER) {
            if (new_path.str.size() == 0)
                new_path.str += it->text;
            else {
                _timestamp();
                std::cerr << "invalid number of identifiers for location_path in " << _path << ":"
                          << it->line_number << "\n";
                exit(EXIT_FAILURE);
            }
        } else if (it->type == OPERATOR && it->text == "*") {
            if (it + 1 != v_token.end() && (it + 1)->text == "{") {
                if (new_path.wildcard != PREFIX)
                    new_path.wildcard = POSTFIX;
                else {
                    _unexpected_operator(it);
                }
            } else {
                _unexpected_operator(it);
            }
        } else {
            _unexpected_operator(it);
        }
    }
    if (it == v_token.end() || it->text == "}") {
        _unexpected_file_ending(it);
    }

    if (v_path_temp.size() > 0) {
        for (std::vector<LocationPath>::iterator it = v_path_temp.begin(); it != v_path_temp.end();
             ++it) {
            if (new_path.wildcard == POSTFIX) {
                it->str += new_path.str;
                it->wildcard = POSTFIX;
            }
            v_path.push_back(*it);
        }
    } else {
        v_path.push_back(new_path);
    }
}

void Interpreter::_parse_bool(std::vector<Token>::const_iterator &it, bool &identifier) {
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

std::string Interpreter::_timestamp() const {
    std::time_t t = time(NULL);
    struct tm  *time_master = localtime(&t);
    std::string ret;

    std::cerr << time_master->tm_year + 1900 << "/";
    if (time_master->tm_mon + 1 < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_mon + 1 << "/";
    if (time_master->tm_mday < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_mday << " ";
    if (time_master->tm_hour < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_hour << ":";
    if (time_master->tm_min < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_min << ":";
    if (time_master->tm_sec < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_sec << " ";

    return ret;
}

void Interpreter::_invalid_directive(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "\"" << *_last_directive << "\" directive is not allowed here in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_file_ending(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "unexpected end of file, expecting \"}\" in " << _path << ":"
              << (it - 1)->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_operator(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "unexpected operator \"" << it->text << "\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_none_terminated_directive(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in " << _path
              << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_bool_argument(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "invalid value \"" << it->text << "\" in \"" << *_last_directive
              << "\" directive, it must be \"on\" or \"off\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const {
    _timestamp();
    std::cerr << "invalid number of arguments in \"" << *_last_directive << "\" directive in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_missing_opening(std::vector<Token>::const_iterator &it, const char &op) const {
    _timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" has no opening \"" << op << "\" in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

}  // namespace config