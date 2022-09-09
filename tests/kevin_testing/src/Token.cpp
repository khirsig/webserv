/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:31:03 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 09:31:40 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"

namespace ft {

void Token::debug_print() const {
    std::cout << "Token(" << token_type_string[type] << ", \"" << text << "\", " << line_number
              << ")"
              << "\n";
}

}  // namespace ft
