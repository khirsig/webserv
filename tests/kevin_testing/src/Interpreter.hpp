/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:26:34 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/16 09:58:30 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#include "Server.hpp"
#include "Token.hpp"

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
    bool _parse_listen(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       Listen &identifier);
    void _parse_port(std::vector<Token>::const_iterator &it, const std::string &str,
                     in_port_t &port);
    void _parse_error_page(const std::vector<Token>           &v_token,
                           std::vector<Token>::const_iterator &it, ErrorPage &identifier);
    void _parse_redirect(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         Redirect &identifier);
    void _parse_bytes(std::vector<Token>::const_iterator &it, std::uint64_t &identifier);
    void _parse_location_path(const std::vector<Token>           &v_token,
                              std::vector<Token>::const_iterator &it,
                              std::vector<LocationPath>          &v_path);
    void _parse_bool(std::vector<Token>::const_iterator &it, bool &identifier);

    std::string _timestamp() const;

    void _invalid_directive(std::vector<Token>::const_iterator &it) const;
    void _unexpected_file_ending(std::vector<Token>::const_iterator &it) const;
    void _none_terminated_directive(std::vector<Token>::const_iterator &it) const;
    void _invalid_bool_argument(std::vector<Token>::const_iterator &it) const;
    void _invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const;
    void _missing_opening(std::vector<Token>::const_iterator &it, const char &op) const;
};

}  // namespace config