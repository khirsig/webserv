/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:13:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/08 14:59:54 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace ft {

enum TokenType { WHITESPACE, IDENTIFIER, OPERATOR, COMMENT };

static const char *token_type_string[] = {"WHITESPACE", "IDENTIFIER", "OPERATOR", "COMMENT"};

class Token {
   public:
    enum TokenType type;
    std::string    text;
    std::size_t    line_number;

    void debug_print() const;
};

class Tokenizer {
   public:
    std::vector<Token> parse(const std::string &input_file);

   private:
    void _end_token(Token &current_token, std::vector<Token> &v_token);
};

}  // namespace ft
