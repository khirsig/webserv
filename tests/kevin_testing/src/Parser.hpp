/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:26:34 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/14 13:24:49 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <iostream>
#include <vector>

#include "Server.hpp"
#include "Token.hpp"

namespace config {

class Parser {
   public:
    Parser(const std::string &file_path) : _path(file_path) {}

    void parse(const std::vector<Token> &v_token, std::vector<Server> &v_server);

   private:
    const std::string  _path;
    const std::string *_last_directive;

    Server _parse_server(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it);
    Location _parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it);
    void     _parse_identifier(const std::vector<Token>           &v_token,
                               std::vector<Token>::const_iterator &it,
                               std::vector<std::string>           &v_identifier);
    void     _parse_identifier(std::vector<Token>::const_iterator &it, std::string &identifier);
    void _parse_location(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         std::vector<LocationPath> &v_path);
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