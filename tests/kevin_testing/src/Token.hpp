/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:29:30 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 09:31:03 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

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

}