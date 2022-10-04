/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:31 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/04 15:17:08 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

namespace file {

Handler::Handler(const std::string &path) {
    _path = path;
    _file.open(_path);
    if (!_file.is_open()) {
        throw 1;
    }
    _file.seekg(0, _file.end);
    _max_size = _file.tellg();
    _file.seekg(0, _file.beg);
    _read_size = 0;
}

Handler::~Handler() {
    if (_file.is_open()) {
        _file.close();
    }
}

void Handler::read(const std::size_t buffer_size) {
    if (_file) {
        char *buffer = new char[buffer_size];

        _file.read(buffer, buffer_size);
        _read_size += _file.gcount();

        delete[] buffer;
    } else {
        _max_size = 0;
        _file.close();
    }
}

std::size_t Handler::max_size() const { return _max_size; }

std::size_t Handler::read_size() const { return _read_size; }

std::size_t Handler::left_size() const { return _max_size - _read_size; }

}  // namespace file