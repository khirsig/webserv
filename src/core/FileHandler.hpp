#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "ByteBuffer.hpp"

namespace core {

class FileHandler {
   private:
    std::string  _path;
    std::fstream _file;
    std::size_t  _max_size;
    std::size_t  _read_size;

   public:
    FileHandler();
    ~FileHandler();

    bool   init(const std::string &path);
    size_t read(char *buf, const std::size_t buffer_size);

    bool is_open() const;

    std::size_t max_size() const;
    std::size_t read_size() const;
    std::size_t left_size() const;

    const std::string &get_path() const;
};

}  // namespace core