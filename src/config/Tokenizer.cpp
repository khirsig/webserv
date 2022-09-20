/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:22:48 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/20 11:41:34 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"

namespace config {

void Tokenizer::parse(std::vector<Token> &v_token, const std::string &input_file) {
    Token current_token;

    current_token.line_number = 1;
    current_token.type = WHITESPACE;

    for (std::string::size_type i = 0; i < input_file.size(); ++i) {
        switch (input_file[i]) {
            case '{':
            case '}':
            case '(':
            case ')':
            case ';':
            case '*':
            case '|':
                if (current_token.type != COMMENT && current_token.type != ESCAPE) {
                    _end_token(v_token, current_token);
                    current_token.type = OPERATOR;
                    current_token.text.append(1, input_file[i]);
                    _end_token(v_token, current_token);
                } else if (current_token.type == ESCAPE) {
                    current_token.type = IDENTIFIER;
                    current_token.text.append(1, input_file[i]);
                }
                break;

            case ' ':
            case '\t':
                if (current_token.type != COMMENT && current_token.type != ESCAPE)
                    _end_token(v_token, current_token);
                else if (current_token.type == ESCAPE) {
                    current_token.type = IDENTIFIER;
                    current_token.text.append(1, input_file[i]);
                }
                break;

            case '\r':
            case '\n':
                _end_token(v_token, current_token);
                ++current_token.line_number;
                break;

            case '#':
                current_token.type = COMMENT;
                break;

            case '\\':
                if (current_token.type != ESCAPE && current_token.type != COMMENT)
                    current_token.type = ESCAPE;
                else if (current_token.type != COMMENT) {
                    current_token.type = IDENTIFIER;
                    current_token.text.append(1, input_file[i]);
                }

                break;

            default:
                if (current_token.type == WHITESPACE) {
                    _end_token(v_token, current_token);
                    current_token.type = IDENTIFIER;
                    current_token.text.append(1, input_file[i]);
                } else if (current_token.type == ESCAPE) {
                    current_token.type = IDENTIFIER;
                    current_token.text.append(1, input_file[i]);
                } else {
                    current_token.text.append(1, input_file[i]);
                }
                break;
        }
    }
    _end_token(v_token, current_token);
}

void Tokenizer::_end_token(std::vector<Token> &v_token, Token &current_token) {
    if (current_token.type != WHITESPACE && current_token.type != COMMENT) {
        v_token.push_back(current_token);
    }
    current_token.type = WHITESPACE;
    current_token.text.erase();
}

}  // namespace config
