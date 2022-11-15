#pragma once

#include <iostream>
#include <vector>

namespace core {

class ByteBuffer : public std::vector<std::uint8_t> {
   private:
    size_t _pos;

   public:
    ByteBuffer(std::size_t size = 1024);
    ByteBuffer(const ByteBuffer &buf);
    ~ByteBuffer();

    void append(const char *str, std::size_t n);
    void append(const char *str);
    void append(ByteBuffer *str);
    bool equal(ByteBuffer::iterator pos, const char *str, std::size_t n);

    size_t pos() const;
    void   set_pos(size_t new_pos);

    ByteBuffer &operator+=(const ByteBuffer &buf);
    ByteBuffer &operator+=(std::uint8_t c);

    friend std::ostream &operator<<(std::ostream &os, const ByteBuffer &bb);
};

ByteBuffer operator+(const ByteBuffer &lhs, const ByteBuffer &rhs);

}  // namespace core
