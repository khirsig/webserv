/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:25:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/11/14 14:55:40 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interpreter.hpp"

#include "../core/ByteBuffer.hpp"
#include "../settings.hpp"
#include "../utils/get_cwd.hpp"
#include "../utils/str_to_num.hpp"
#include "../utils/timestamp.hpp"

namespace config {

void Interpreter::parse(const std::vector<Token> &v_token, std::vector<Server> &v_server) {
    for (std::vector<Token>::const_iterator it = v_token.begin(); it != v_token.end(); ++it) {
        _last_directive = &(it->text);
        if (it->text == "server" && it->type == IDENTIFIER) {
            Server new_server;
            if (_parse_server(v_token, it, new_server))
                v_server.insert(v_server.begin(), new_server);
            else
                v_server.push_back(new_server);
        } else {
            _invalid_directive(it);
        }
    }
}

bool Interpreter::_parse_server(const std::vector<Token>           &v_token,
                                std::vector<Token>::const_iterator &it, Server &new_server) {
    _increment_token(v_token, it);

    bool is_default_server = false;
    bool client_max_size_set = false;

    if (it->text == "{" && it->type == OPERATOR) {
        _increment_token(v_token, it);

        for (; it != v_token.end() && it->text != "}"; ++it) {
            _last_directive = &(it->text);
            if (*_last_directive == "listen") {
                core::Address new_listen;
                if (_parse_listen(v_token, it, new_listen))
                    is_default_server = true;
                new_server.v_listen.push_back(new_listen);
            } else if (*_last_directive == "server_name") {
                _parse_string(v_token, it, new_server.v_server_name);
            } else if (*_last_directive == "error_page") {
                _parse_error_page(v_token, it, new_server.m_error_codes);
            } else if (*_last_directive == "client_max_body_size") {
                if (client_max_size_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bytes(v_token, it, new_server.client_max_body_size);
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
    return is_default_server;
}

Location Interpreter::_parse_location(const std::vector<Token>           &v_token,
                                      std::vector<Token>::const_iterator &it) {
    _increment_token(v_token, it);

    Location new_location;
    if (it->type == IDENTIFIER) {
        _parse_location_path(v_token, it, new_location.path);
    } else {
        _missing_opening(it, '{');
    }

    bool dir_listing_set = false;
    bool acc_methods_set = false;
    bool client_max_size_set = false;

    if (it->text == "{" && it->type == OPERATOR) {
        _increment_token(v_token, it);

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
                    _parse_string(v_token, it, new_location.root);
            } else if (*_last_directive == "index") {
                _parse_string(v_token, it, new_location.v_index);
            } else if (*_last_directive == "cgi_pass") {
                CgiPass new_pass;
                _parse_cgi_pass(v_token, it, new_pass);
                new_location.v_cgi_pass.push_back(new_pass);
            } else if (*_last_directive == "directory_listing") {
                if (dir_listing_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bool(v_token, it, new_location.directory_listing);
                    dir_listing_set = true;
                }
            } else if (*_last_directive == "client_max_body_size") {
                if (client_max_size_set) {
                    _directive_already_set(it);
                } else {
                    _parse_bytes(v_token, it, new_location.client_max_body_size);
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
    _increment_token(v_token, it);

    for (; it != v_token.end(); ++it) {
        if (it->type == OPERATOR) {
            if (it->text == ";" && !v_identifier.empty())
                break;
            else {
                _unexpected_operator(it);
            }
        }
        std::string str(it->text);
        if (*_last_directive == "accepted_methods") {
            if (!(str == "GET" || str == "POST" || str == "DELETE" || str == "HEAD")) {
                _wrong_method(it, str);
            }
        }
        v_identifier.push_back(str);
        str.erase();
    }
    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Interpreter::_parse_string(const std::vector<Token>           &v_token,
                                std::vector<Token>::const_iterator &it, std::string &identifier) {
    _increment_token(v_token, it);

    if (it->type == OPERATOR)
        _unexpected_operator(it);
    identifier = it->text;
    if (*_last_directive == "root" && identifier.back() != '/')
        identifier += '/';
    _increment_token(v_token, it);

    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Interpreter::_parse_cgi_pass(const std::vector<Token>           &v_token,
                                  std::vector<Token>::const_iterator &it, CgiPass &identifier) {
    _increment_token(v_token, it);

    if (it->type == OPERATOR)
        _unexpected_operator(it);
    identifier.type = it->text;
    _increment_token(v_token, it);

    if (it->type == OPERATOR)
        _unexpected_operator(it);
    identifier.path = utils::get_absolute_path(it->text);
    _increment_token(v_token, it);

    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}
bool Interpreter::_parse_listen(const std::vector<Token>           &v_token,
                                std::vector<Token>::const_iterator &it, core::Address &identifier) {
    _increment_token(v_token, it);
    if (it->type == OPERATOR)
        _unexpected_operator(it);

    bool default_server = false;

    std::size_t seperator_index = it->text.find_first_of(':');
    if (seperator_index != it->text.find_last_of(':')) {
        _multiple_operator_used(it, ':');
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
    _increment_token(v_token, it);

    for (; it != v_token.end() && it->text != ";"; ++it) {
        if (it->text == "default_server") {
            default_server = true;
        } else {
            _invalid_parameter(it);
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
    size_t i = 0;
    if (str.empty() || !utils ::str_to_num_dec(str, i) || i > 65535)
        _invalid_port(it, str);
    port = htons(i);
}

void Interpreter::_parse_error_page(const std::vector<Token>           &v_token,
                                    std::vector<Token>::const_iterator &it,
                                    std::map<int, http::error_page_t>  &m_error_page) {
    _increment_token(v_token, it);

    std::vector<std::uint32_t> v_code;

    for (; it != v_token.end() && it->text != ";"; ++it) {
        if (it->text.find_first_not_of("0123456789") == std::string::npos) {
            uint32_t new_code;
            std::stringstream(it->text) >> new_code;
            if (!http::is_valid_error_code(new_code)) {
                _invalid_error_code(it, new_code);
            }
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
        _could_not_open_file(it);
    }
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    std::string content(file_stream.str());

    http::error_page_t new_error_page;
    new_error_page.content.append(content.c_str(), content.size());

    if (it->text.find(".html", it->text.size() - 5) != std::string::npos)
        new_error_page.content_type = "text/html";
    else
        new_error_page.content_type = "text/plain";

    for (std::size_t i = 0; i < v_code.size(); ++i) {
        m_error_page.insert(std::make_pair(v_code[i], new_error_page));
    }

    _increment_token(v_token, it);

    if (it->text != ";")
        _none_terminated_directive(it);
}

void Interpreter::_parse_redirect(const std::vector<Token>           &v_token,
                                  std::vector<Token>::const_iterator &it, Redirect &identifier) {
    _increment_token(v_token, it);

    std::vector<Token>::const_iterator iter = it;
    std::uint32_t                      count = 0;
    for (; iter != v_token.end() && iter->text != ";"; ++iter) {
        if (iter->type == OPERATOR)
            _unexpected_operator(iter);
        count++;
    }
    if (iter == v_token.end()) {
        _unexpected_eof(it);
    }
    if (count != 3)
        _invalid_directive_argument_amount(iter);

    if (it->text.find_first_not_of("0123456789") == std::string::npos) {
        std::stringstream(it->text) >> identifier.status_code;
        const uint32_t *end_ptr = allowed_redir + sizeof(allowed_redir) / sizeof(uint32_t);
        if (std::find(allowed_redir, end_ptr, identifier.status_code) == end_ptr) {
            _invalid_status_code(it, identifier.status_code);
        }
    } else {
        _invalid_status_code(it, identifier.status_code);
    }
    _increment_token(v_token, it);

    identifier.origin = it->text;
    _increment_token(v_token, it);

    identifier.direction = it->text;
    _increment_token(v_token, it);

    if (it->text != ";")
        _none_terminated_directive(it);
}

void Interpreter::_parse_bytes(const std::vector<Token>           &v_token,
                               std::vector<Token>::const_iterator &it, std::uint64_t &identifier) {
    _increment_token(v_token, it);

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
        _invalid_size_identifier(it, num);
        exit(EXIT_FAILURE);
    }

    if ((num.size() > 3 && multiplier == 1000000000) || (num.size() > 6 && multiplier == 1000000) ||
        (num.size() > 9 && multiplier == 1000) || (num.size() > 12 && multiplier == 1)) {
        _numeric_overflow(it, num);
    }

    if (num.find_first_not_of("0123456789") != std::string::npos) {
        _numeric_char_expected(it, num);
    }

    char *p_end;
    identifier = strtol(num.c_str(), &p_end, 10) * multiplier;

    if (identifier > CLIENT_MAX_BODY_SIZE)
        _numeric_overflow(it, num);

    _increment_token(v_token, it);

    if (it->text != ";") {
        _none_terminated_directive(it);
    }
}

void Interpreter::_parse_location_path(const std::vector<Token>           &v_token,
                                       std::vector<Token>::const_iterator &it,
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
                    _invalid_path(it);
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
                    _invalid_path(it);
                } else {
                    location_path += "..";
                    location_path += c;
                    state = SEGMENT;
                }
                break;
        }
    }

    if (location_path.size() > 1 && location_path[location_path.size() - 1] == '/')
        location_path.erase(location_path.size() - 1);

    _increment_token(v_token, it);

    if (it->text != "{") {
        _missing_opening(it, '{');
    }
}

void Interpreter::_parse_bool(const std::vector<Token>           &v_token,
                              std::vector<Token>::const_iterator &it, bool &identifier) {
    _increment_token(v_token, it);

    if (it->text == "on") {
        identifier = true;
    } else if (it->text == "off") {
        identifier = false;
    } else {
        _invalid_bool_argument(it);
    }
    _increment_token(v_token, it);
    if (it->text != ";") {
        _invalid_directive(it);
    }
}

// Function that increments the iterator and checks if it is at the end of the vector
void Interpreter::_increment_token(const std::vector<Token>           &v_token,
                                   std::vector<Token>::const_iterator &it) {
    ++it;
    if (it == v_token.end())
        _unexpected_eof(it);
}

void Interpreter::_invalid_directive(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " \"" << *_last_directive << "\" directive is not allowed here in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_directive_already_set(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " duplicate entry for directive \"" << *_last_directive << "\" in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_file_ending(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " unexpected end of file, expecting \"}\" in " << _path << ":"
              << (it - 1)->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_eof(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " unexpected end of file in " << _path << ":" << (it - 1)->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_unexpected_operator(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " unexpected operator \"" << it->text << "\" in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_none_terminated_directive(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " directive \"" << *_last_directive << "\" is not terminated by \";\" in " << _path
              << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_bool_argument(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid value \"" << it->text << "\" in \"" << *_last_directive
              << "\" directive, it must be \"on\" or \"off\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid number of arguments in \"" << *_last_directive << "\" directive in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_missing_opening(std::vector<Token>::const_iterator &it, const char &op) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " directive \"" << *_last_directive << "\" has no opening \"" << op << "\" in "
              << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_status_code(std::vector<Token>::const_iterator &it,
                                       const uint32_t                     &status_code) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid status code \"" << status_code << "\" for \"" << *_last_directive
              << "\" in " << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_path(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid path \"" << it->text << "\" for directive \"" << *_last_directive
              << "\" in " << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_error_code(std::vector<Token>::const_iterator &it,
                                      const int32_t                      &code) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid error_code \"" << code << "\""
              << " for directive \"" << *_last_directive << "\" in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_could_not_open_file(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " file at \"" << it->text << "\""
              << " for directive \"" << *_last_directive << "\" could not be opened in " << _path
              << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_port(std::vector<Token>::const_iterator &it,
                                const std::string                  &port_str) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid port in \"" << port_str << "\" of the \"" << *_last_directive
              << "\" directive in " << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_numeric_char_expected(std::vector<Token>::const_iterator &it,
                                         const std::string                  &num) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid character for numeric value in \"" << num << "\" of the \""
              << *_last_directive << "\" directive in " << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_size_identifier(std::vector<Token>::const_iterator &it,
                                           const std::string                  &num) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid size identifier in \"" << num << "\" of the \"" << *_last_directive
              << "\" directive in " << _path << ":" << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_numeric_overflow(std::vector<Token>::const_iterator &it,
                                    const std::string                  &num) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " numeric overflow (max " << CLIENT_MAX_BODY_SIZE << " bytes) in \"" << num
              << "\" of the \"" << *_last_directive << "\" directive in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_invalid_parameter(std::vector<Token>::const_iterator &it) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " invalid parameter \"" << it->text << "\" in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_wrong_method(std::vector<Token>::const_iterator &it,
                                const std::string                  &method) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " \"" << method << "\" is not allowed as \"accepted_method\" in " << _path << ":"
              << it->line_number << "\n";
    exit(EXIT_FAILURE);
}

void Interpreter::_multiple_operator_used(std::vector<Token>::const_iterator &it,
                                          const char                         &op) const {
    utils::print_timestamp(std::cerr);
    std::cerr << " multiple \"" << op << "\" operator used in " << _path << ":" << it->line_number
              << "\n";
    exit(EXIT_FAILURE);
}

}  // namespace config