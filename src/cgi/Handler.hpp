/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:20:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/06 15:33:25 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"

namespace cgi {

class Handler {
   public:
    Handler();
    ~Handler();

    void    execute(ByteBuffer &path, ByteBuffer &body, std::map<std::string, std::string> env);
    int32_t get_fd() const;

   private:
    int32_t _read_fd;
    pid_t   _id;

    void _run_program(ByteBuffer &path, std::map<std::string, std::string> env);
};
}  // namespace cgi

// Pipe 1
// read -> dup2 auf std::in child
// write -> body in schreibende pipe

// Pipe 2
// read -> public in cgi als read end
// write -> dup2 auf std::out child

// CONTENT_LENGTH
// CONTENT_TYPE
//