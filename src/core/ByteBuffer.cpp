/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ByteBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:58:19 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/21 14:51:53 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ByteBuffer.hpp"

namespace core {

void ByteBuffer::append(const char *str, std::size_t n) {
    if (str == NULL)
        return;
    reserve(size() + n);
    for (std::size_t i = 0; i < n; ++i) {
        push_back(str[i]);
    }
}

bool ByteBuffer::equal(ByteBuffer::iterator pos, const char *str, std::size_t n) {
    size_t i = 0;
    for (; pos != end() && i < n; pos++, i++) {
        if (*pos != str[i])
            return false;
    }
    return i == n;
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

std::ostream &operator<<(std::ostream &os, const ByteBuffer &bb) {
    for (size_t i = 0; i < bb.size(); ++i) {
        os << bb[i];
    }
    return os;
}

}  // namespace core