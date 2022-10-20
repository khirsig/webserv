/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 12:35:49 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/20 19:50:17 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../core/ByteBuffer.hpp"

namespace file {

class Handler {
   public:
    Handler();
    ~Handler();

    void   init(const std::string &path);
    size_t read(char *buf, const std::size_t buffer_size);

    std::size_t max_size() const;
    std::size_t read_size() const;
    std::size_t left_size() const;

    const std::string& get_path() const;

   private:
    std::string  _path;
    std::fstream _file;
    std::size_t  _max_size;
    std::size_t  _read_size;
};

}  // namespace file