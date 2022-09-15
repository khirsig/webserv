/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:13:07 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/15 09:36:45 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

namespace config {

class Tokenizer {
   public:
    void parse(std::vector<Token> &v_token, const std::string &input_file);

   private:
    void _end_token(Token &current_token, std::vector<Token> &v_token);
};

}  // namespace config
