/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:26:34 by khirsig           #+#    #+#             */
/*   Updated: 2022/11/03 15:17:09 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../core/Address.hpp"
#include "../http/ErrorPages.hpp"
#include "../http/StatusCodes.hpp"
#include "Server.hpp"
#include "Token.hpp"

static const uint32_t allowed_redir[] = {HTTP_MOVED_PERMANENTLY, HTTP_FOUND,
                                         HTTP_TEMPORARY_REDIRECT, HTTP_PERMANENT_REDIRECT};

enum state_location_path { START, SLASH, FIRST_DOT, SECOND_DOT, SEGMENT };

namespace config {

class Interpreter {
   public:
    Interpreter(const std::string &file_path) : _path(file_path) {}

    void parse(const std::vector<Token> &v_token, std::vector<Server> &v_server);

   private:
    const std::string  _path;
    const std::string *_last_directive;

    Server _parse_server(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it);
    Location _parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it);
    void _parse_string(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       std::vector<std::string> &v_identifier);
    void _parse_string(std::vector<Token>::const_iterator &it, std::string &identifier);
    void _parse_cgi_pass(std::vector<Token>::const_iterator &it, CgiPass &identifier);
    bool _parse_listen(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       core::Address &identifier);
    void _parse_port(std::vector<Token>::const_iterator &it, const std::string &str,
                     in_port_t &port);
    void _parse_error_page(const std::vector<Token>           &v_token,
                           std::vector<Token>::const_iterator &it);
    void _parse_redirect(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         Redirect &identifier);
    void _parse_bytes(std::vector<Token>::const_iterator &it, std::uint64_t &identifier);
    void _parse_location_path(std::vector<Token>::const_iterator &it, std::string &location_path);
    void _parse_bool(std::vector<Token>::const_iterator &it, bool &identifier);

    void _invalid_directive(std::vector<Token>::const_iterator &it) const;
    void _directive_already_set(std::vector<Token>::const_iterator &it) const;
    void _unexpected_file_ending(std::vector<Token>::const_iterator &it) const;
    void _unexpected_operator(std::vector<Token>::const_iterator &it) const;
    void _none_terminated_directive(std::vector<Token>::const_iterator &it) const;
    void _invalid_bool_argument(std::vector<Token>::const_iterator &it) const;
    void _invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const;
    void _missing_opening(std::vector<Token>::const_iterator &it, const char &op) const;
};

}  // namespace config