/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:26:34 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/12 08:49:35 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>

#include "Server.hpp"
#include "Token.hpp"

namespace ft {

class Parser {
   public:
    Parser(const std::string file_path) : _path(file_path) {}

    void parse(const std::vector<Token> &v_token, std::vector<Server> &v_server);

   private:
    std::string _path;

    Server _parse_server(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it);
    Location _parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it);
    void     _parse_identifier(const std::vector<Token>           &v_token,
                               std::vector<Token>::const_iterator &it,
                               std::vector<std::string> &v_identifier, std::string err_code);
    void     _parse_identifier(std::vector<Token>::const_iterator &it, std::string &identifier,
                               std::string err_code);
    void _parse_location(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         std::vector<LocationPath> &v_path);
    void _parse_bool(std::vector<Token>::const_iterator &it, bool &identifier,
                     std::string err_code);
    void _exit_error(const Token &false_token, const std::string expected_text,
                     const std::size_t expected_type);
};
}  // namespace ft