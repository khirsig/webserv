/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ByteBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:51:44 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/20 14:44:24 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

namespace core {

class ByteBuffer : public std::vector<std::uint8_t> {
   public:
    void append(const char *str, std::size_t n);

    ByteBuffer &operator+=(const ByteBuffer &buf);
    ByteBuffer &operator+=(std::uint8_t c);
};

ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);
ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);

}  // namespace core