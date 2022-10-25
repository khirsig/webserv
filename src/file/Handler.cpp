/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hepple <hepple@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:31 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/25 17:58:13 by hepple           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"
#include "../http/httpStatusCodes.hpp"

namespace file {

Handler::Handler() : _max_size(0), _read_size(0) {}

Handler::~Handler() {
    if (_file.is_open()) {
        _file.close();
    }
}

bool Handler::init(const std::string &path) {
    _path = path;
    _file.open(path);
    if (!_file.is_open()) {
        if (errno == 13)
            throw HTTP_FORBIDDEN;
        else if (errno == 21)
            return false;
        else
            throw HTTP_NOT_FOUND;
    }
    _file.seekg(0, _file.end);
    _max_size = _file.tellg();
    _file.seekg(0, _file.beg);
    return true;
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