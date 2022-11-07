#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../settings.hpp"
#include "ByteBuffer.hpp"

namespace core {

class FileHandler {
   private:
    std::string  _path;
    std::fstream _file;
    std::size_t  _max_size;
    std::size_t  _read_size;
    char        *_buf;

   public:
    static const size_t BUF_SIZE = FILE_BUF_SIZE;

    FileHandler();
    FileHandler(const FileHandler &other);
    ~FileHandler();

    bool   init(const std::string &path);
    size_t read(size_t max_len);
    void   close();

    bool is_open() const;

    std::size_t max_size() const;
    std::size_t read_size() const;
    std::size_t left_size() const;

    const std::string &path() const;
    const char        *buf() const;
};

}  // namespace core
