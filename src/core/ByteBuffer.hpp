#pragma once

#include <iostream>
#include <vector>

namespace core {

class ByteBuffer : public std::vector<std::uint8_t> {
   public:
    std::size_t pos;

    ByteBuffer(std::size_t size = 1024);
    ByteBuffer(const ByteBuffer &buf);
    ~ByteBuffer();

    void append(const char *str, std::size_t n);
    void append(const char *str);
    void append(ByteBuffer *str);
    bool equal(ByteBuffer::iterator pos, const char *str, std::size_t n);

    ByteBuffer &operator+=(const ByteBuffer &buf);
    ByteBuffer &operator+=(std::uint8_t c);

    friend std::ostream &operator<<(std::ostream &os, const ByteBuffer &bb);
};

ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);

}  // namespace core