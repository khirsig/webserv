/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:31 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/20 16:30:02 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

namespace file {

Handler::Handler(const std::string &path) {
    _path = path;
    _file.open(_path);
    if (!_file.is_open()) {
        if (errno == 13)
            throw 403;
        else
            throw 404;
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

void Handler::read(char *buf, const std::size_t buffer_size) {
    if (_file) {
        _file.read(buf, buffer_size);
        _read_size += _file.gcount();
    } else {
        _file.close();
    }
}

std::size_t Handler::max_size() const { return _max_size; }

std::size_t Handler::read_size() const { return _read_size; }

std::size_t Handler::left_size() const { return _max_size - _read_size; }

}  // namespace file