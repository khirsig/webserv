/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/25 13:11:36 by khirsig          ###   ########.fr       */
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
    bool   client_max_size_set = false;
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
                _parse_error_page(v_token, it);
            } else if (*_last_directive == "client_max_body_size") {
                if (client_max_size_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bytes(it, new_server.client_max_body_size);
                    client_max_size_set = true;
                }
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
    if (it->type == IDENTIFIER || it->text == "(") {
        _parse_location_path(it, new_location.path);
    } else {
        std::cout << "No identifier for location in " << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }

    bool dir_listing_set = false;
    bool acc_methods_set = false;
    bool client_max_size_set = false;

    if (it->text == "{" && it->type == OPERATOR) {
        ++it;
        for (; it != v_token.end() && it->text != "}"; ++it) {
            _last_directive = &(it->text);
            if (*_last_directive == "accepted_methods") {
                if (acc_methods_set) {
                    _directive_already_set(it);
                } else {
                    _parse_string(v_token, it, new_location.v_accepted_method);
                    acc_methods_set = true;
                }
            } else if (*_last_directive == "redirect") {
                Redirect new_redirect;
                _parse_redirect(v_token, it, new_redirect);
                new_location.v_redirect.push_back(new_redirect);
            } else if (*_last_directive == "root") {
                if (new_location.root.size() != 0)
                    _directive_already_set(it);
                else
                    _parse_string(it, new_location.root);
            } else if (*_last_directive == "index") {
                _parse_string(v_token, it, new_location.v_index);
            } else if (*_last_directive == "cgi_pass") {
                CgiPass new_pass;
                _parse_cgi_pass(it, new_pass);
                new_location.v_cgi_pass.push_back(new_pass);
            } else if (*_last_directive == "directory_listing") {
                if (dir_listing_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bool(it, new_location.directory_listing);
                    dir_listing_set = true;
                }
            } else if (*_last_directive == "client_max_body_size") {
                if (client_max_size_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bytes(it, new_location.client_max_body_size);
                    client_max_size_set = true;
                }
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
    if (*_last_directive == "root" && identifier.back() != '/')
        identifier += '/';
    ++it;
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Interpreter::_parse_cgi_pass(std::vector<Token>::const_iterator &it, CgiPass &identifier) {
    ++it;
    if (it->type == OPERATOR)
        _unexpected_operator(it);
    identifier.type = it->text;
    ++it;
    if (it->type == OPERATOR)
        _unexpected_operator(it);
    identifier.path = it->text;
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
                                    std::vector<Token>::const_iterator &it) {
    ++it;
    std::vector<std::uint32_t> v_code;

    for (; it != v_token.end() && it->text != ";"; ++it) {
        if (it->text.find_first_not_of("0123456789") == std::string::npos) {
            std::uint32_t new_code;
            std::stringstream(it->text) >> new_code;
            if (http::g_error_pages.pages.find(new_code) == http::g_error_pages.pages.end()) {
                std::cerr << "invalid error_code \"" << new_code << "\""
                          << " for directive \"" << *_last_directive << "\" in " << _path << ":"
                          << it->line_number << "\n";
                exit(EXIT_FAILURE);
            }
            std::stringstream(it->text) >> new_code;
            v_code.push_back(new_code);
        } else {
            break;
        }
    }
    if (it == v_token.end() || it->text == ";" || v_code.size() <= 0) {
        if (it == v_token.end())
            _unexpected_file_ending(it);
        else
            _invalid_directive_argument_amount(it);
    }

    std::ifstream file;
    file.open(it->text);
    if (!file.is_open()) {
        std::cerr << "file at \"" << it->text << "\""
                  << " for directive \"" << *_last_directive << "\" could not be opened in "
                  << _path << ":" << it->line_number << "\n";
        exit(EXIT_FAILURE);
    }
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    std::string      content(file_stream.str());
    core::ByteBuffer buf;
    buf.append(content.c_str(), content.size());

    std::string content_type;
    if (it->text.find(".html", it->text.size() - 5) != std::string::npos)
        content_type = "text/html";
    else
        content_type = "text/plain";

    for (std::size_t i = 0; i < v_code.size(); ++i) {
        http::g_error_pages.insert_page(v_code[i], buf, content_type);
    }

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
        const uint32_t *end_ptr = allowed_redir + sizeof(allowed_redir) / sizeof(uint32_t);
        if (std::find(allowed_redir, end_ptr, identifier.status_code) == end_ptr) {
            core::timestamp();
            std::cerr << "invalid status code \"" << identifier.status_code << "\" for \""
                      << *_last_directive << "\" in " << _path << ":" << it->line_number << "\n";
            exit(EXIT_FAILURE);
        }
    } else {
        core::timestamp();
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

    if ((num.size() > 3 && multiplier == 1000000000) || (num.size() > 6 && multiplier == 1000000) ||
        (num.size() > 9 && multiplier == 1000) || (num.size() > 12 && multiplier == 1)) {
        std::cerr << "numeric amount too high. maximum allowed file-size = 999gb\n";
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

void Interpreter::_parse_location_path(std::vector<Token>::const_iterator &it,
                                       std::string                        &location_path) {
    if (it->type == OPERATOR) {
        _unexpected_operator(it);
    }

    state_location_path state = START;
    char                c;
    for (std::size_t i = 0; i < it->text.size(); ++i) {
        c = it->text[i];
        switch (state) {
            case START:
                if (c == '/') {
                    location_path += c;
                    state = SLASH;
                    break;
                } else {
                    core::timestamp();
                    std::cerr << "invalid path \"" << it->text << "\" for directive \""
                              << *_last_directive << "\" in " << _path << ":" << it->line_number
                              << "\n";
                    exit(EXIT_FAILURE);
                }
            case SLASH:
                if (c == '/')
                    break;
                else if (c == '.') {
                    state = FIRST_DOT;
                    break;
                } else {
                    location_path += c;
                    state = SEGMENT;
                    break;
                }
            case FIRST_DOT:
                if (c == '/') {
                    state = SLASH;
                    break;
                } else if (c == '.') {
                    state = SECOND_DOT;
                    break;
                } else {
                    location_path += '.';
                    location_path += c;
                    state = SEGMENT;
                    break;
                }
            case SEGMENT:
                location_path += c;
                if (c == '/')
                    state = SLASH;
                break;
            case SECOND_DOT:
                if (c == '/') {
                    core::timestamp();
                    std::cerr << "invalid path \"" << it->text << "\" for directive \""
                              << *_last_directive << "\" in " << _path << ":" << it->line_number
                              << "\n";
                    exit(EXIT_FAILURE);
                } else {
                    location_path += "..";
                    location_path += c;
                    state = SEGMENT;
                }
                break;
        }
    }
    if (location_path[location_path.size() - 1] != '/')
        location_path += '/';

    ++it;
    if (it->text != "{") {
        _missing_opening(it, '{');
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

void Interpreter::_invalid_directive(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "\"" << *_last_directive << "\" directive is not allowed here in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_directive_already_set(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "duplicate entry for directive \"" << *_last_directive << "\" in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_file_ending(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "unexpected end of file, expecting \"}\" in " << _path << ":"
              << (it - 1)->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_operator(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "unexpected operator \"" << it->text << "\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_none_terminated_directive(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" is not terminated by \";\" in " << _path
              << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_bool_argument(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "invalid value \"" << it->text << "\" in \"" << *_last_directive
              << "\" directive, it must be \"on\" or \"off\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const {
    core::timestamp();
    std::cerr << "invalid number of arguments in \"" << *_last_directive << "\" directive in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_missing_opening(std::vector<Token>::const_iterator &it, const char &op) const {
    core::timestamp();
    std::cerr << "directive \"" << *_last_directive << "\" has no opening \"" << op << "\" in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

}  // namespace config