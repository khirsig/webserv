/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ByteBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:51:44 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/21 14:42:32 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>

namespace core {

class ByteBuffer : public std::vector<std::uint8_t> {
   public:
    void append(const char *str, std::size_t n);
    bool equal(ByteBuffer::iterator pos, const char *str, std::size_t n);

    ByteBuffer &operator+=(const ByteBuffer &buf);
    ByteBuffer &operator+=(std::uint8_t c);

    friend std::ostream &operator<<(std::ostream &os, const ByteBuffer &bb);
};

ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);
ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);

}  // namespace core