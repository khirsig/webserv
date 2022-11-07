#include "FileHandler.hpp"

#include "../http/status_codes.hpp"

namespace core {

FileHandler::FileHandler() : _max_size(0), _read_size(0) { _buf = new char[BUF_SIZE]; }

FileHandler::FileHandler(const FileHandler &other) {
    if (this != &other) {
        _max_size = 0;
        _read_size = 0;
        _buf = new char[BUF_SIZE];
    }
}

FileHandler::~FileHandler() {
    delete[] _buf;
    if (_file.is_open()) {
        _file.close();
    }
}

bool FileHandler::init(const std::string &path) {
    _path = path;
    _file.open(path);
    if (!_file.is_open()) {
        if (errno == EACCES)
            throw HTTP_FORBIDDEN;
        else if (errno == EISDIR)
            return false;
        else
            throw HTTP_NOT_FOUND;
    }
    _file.seekg(0, _file.end);
    _max_size = _file.tellg();
    _file.seekg(0, _file.beg);
    return true;
}

size_t FileHandler::read(size_t max_len) {
    if (_file) {
        size_t to_read_len = max_len < BUF_SIZE ? max_len : BUF_SIZE;
        _file.read(_buf, to_read_len);
        size_t read_bytes = _file.gcount();
        _read_size += read_bytes;
        return read_bytes;
    } else {
        _file.close();
        return 0;
    }
}

void FileHandler::close() {
    if (_file.is_open()) {
        _file.close();
    }
    _max_size = 0;
    _read_size = 0;
}

bool FileHandler::is_open() const { return _file.is_open(); }

std::size_t FileHandler::max_size() const { return _max_size; }

std::size_t FileHandler::read_size() const { return _read_size; }

std::size_t FileHandler::left_size() const { return _max_size - _read_size; }

const std::string &FileHandler::path() const { return _path; }

const char *FileHandler::buf() const { return _buf; }

}  // namespace core
