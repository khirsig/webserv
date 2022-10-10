/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ByteBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:58:19 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/20 14:33:54 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ByteBuffer.hpp"

namespace core {

void ByteBuffer::append(const char *str, std::size_t n) {
    if (str == NULL)
        return;
    reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        push_back(str[i]);
    }
}

ByteBuffer &ByteBuffer::operator+=(const ByteBuffer &buf) {
    insert(end(), buf.begin(), buf.end());
    return *this;
}

ByteBuffer &ByteBuffer::operator+=(std::uint8_t c) {
    push_back(c);
    return *this;
}

ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs) {
    ByteBuffer ret(lhs);
    ret += rhs;

    return ret;
}

}  // namespace core
