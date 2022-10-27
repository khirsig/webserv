/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:49 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/27 10:16:36 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../core/ByteBuffer.hpp"

namespace file {

class FileHandler {
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

   private:
    std::string  _path;
    std::fstream _file;
    std::size_t  _max_size;
    std::size_t  _read_size;
};

}  // namespace file