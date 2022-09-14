/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:13:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/14 10:21:19 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

namespace ft {

class Tokenizer {
   public:
    std::vector<Token> parse(const std::string &input_file);

   private:
    void _end_token(Token &current_token, std::vector<Token> &v_token);
};

}  // namespace ft
