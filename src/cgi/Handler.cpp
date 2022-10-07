/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:53:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/07 12:19:05 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handler.hpp"

namespace cgi {

Handler::Handler() {}

Handler::~Handler() { close(_read_fd); }

void Handler::execute(core::ByteBuffer &path, core::ByteBuffer &body,
                      std::map<std::string, std::string> &env) {
    int fd[2];

    if (pipe(fd) == -1)
        throw 500;

    _id = fork();

    if (_id == -1) {
        close(fd[0]);
        close(fd[1]);
        throw 500;
    }

    if (_id == 0) {
        _run_program(path, body, env, fd);
    } else {
        _read_fd = fd[0];

        close(fd[1]);
    }
}

int32_t Handler::get_fd() const { return _read_fd; }

void Handler::_run_program(core::ByteBuffer &path, core::ByteBuffer &body,
                           std::map<std::string, std::string> &env, int fd[2]) {
    write(STDIN_FILENO, &body, body.size());
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    // change directory before execve to right path
    // execve
    // after execve parse error log

    char **env_str = _get_env(env);
}

char **Handler::_get_env(std::map<std::string, std::string> &env) {
    for (uint32_t i = 0; i < env_arr_length; ++i) {
        if (env.find(env_string[i]) == env.end()) {
            std::string key = env_string[i];
            std::string val;
            env.insert(std::make_pair<std::string, std::string>(key, val);
        }
    }

    char   **ret = new char *[env.size()];
    uint32_t i = 0;
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
        ret[i] = new char[it->first.length + it->second.length + 2];
        uint32_t j = 0;
        for (uint32_t k = 0; k < it->first.length; ++k, ++j) {
            ret[i][j] = it->first[k];
        }
        ret[i][j] = '=';
        ++j;
        for (uint32_t k = 0; k < it->second.length; ++k, ++j) {
            ret[i][j] = it->second[k];
        }
        ret[i][j] = '\0';
        ++i;
    }

    return ret;
}

}  // namespace cgi