/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:53:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/06 15:13:34 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

namespace cgi {

Handler::Handler() {}

Handler::~Handler() { close(_read_fd); }

void Handler::execute(core::ByteBuffer &path, core::ByteBuffer &body,
                      std::map<std::string, std::string> env) {
    int pipe[2];

    if (pipe(pipe) == -1)
        throw 500;

    _id = fork();

    if (_id == -1)
        throw 500;

    _read_fd = pipe[0];

    close(pipe[1]);
}

int32_t Handler::get_fd() const { return _read_fd; }

void _run_program(core::ByteBuffer &path, core::ByteBuffer &body,
                  std::map<std::string, std::string> env, int pipe[2]) {
    write(STDIN_FILENO, &body, body.size());
    close(pipe[0]);
    dup2(pipe[1], STDOUT_FILENO);
    close(pipe[1]);
    std::string path_str((char *)path);
}

}  // namespace cgi