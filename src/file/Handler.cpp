/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:31 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/25 10:43:08 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

namespace file {

Handler::Handler() : _max_size(0), _read_size(0) {}

Handler::~Handler() {
    if (_file.is_open()) {
        _file.close();
    }
}

void Handler::init(const std::string &path) {
    _path = path;
    _file.open(path);
    if (!_file.is_open()) {
        if (errno == 13)
            throw 403;
        else
            throw 404;
    }
    _file.seekg(0, _file.end);
    _max_size = _file.tellg();
    _file.seekg(0, _file.beg);
}

size_t Handler::read(char *buf, const std::size_t buffer_size) {
    if (_file) {
        _file.read(buf, buffer_size);
        size_t read_bytes = _file.gcount();
        _read_size += read_bytes;
        return read_bytes;
    } else {
        _file.close();
        return 0;
    }
}

bool Handler::is_open() const { return _file.is_open(); }

std::size_t Handler::max_size() const {
    std::cerr << _max_size << "\n";
    return _max_size;
}

std::size_t Handler::read_size() const { return _read_size; }

std::size_t Handler::left_size() const { return _max_size - _read_size; }

const std::string &Handler::get_path() const { return _path; }

}  // namespace file