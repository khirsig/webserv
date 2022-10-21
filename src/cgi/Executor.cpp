/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:53:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/21 13:15:09 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Executor.hpp"

namespace cgi {

Executor::Executor() {}

Executor::~Executor() { close(_read_fd); }

void Executor::execute(std::string &root, std::string &path, std::string &body,
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
        _run_program(root, path, body, env, fd);
    } else {
        _read_fd = fd[0];

        close(fd[1]);
    }
}

int32_t Executor::get_fd() const { return _read_fd; }

void Executor::_run_program(std::string &root, std::string &path, std::string &body,
                            std::map<std::string, std::string> &env, int fd[2]) {
    // std::cout << "path: " << path << "   root: " << root << "    body: " << body << "\n";
    // write(STDIN_FILENO, &body, body.size());
    // close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    // change directory before execve to right path
    // execve
    // after execve parse error log
    chdir(root.c_str());
    body.insert(0, ".");
    char **env_str = _get_env(env);
    char **argv = _get_argv(path, body);
    // std::cout << argv[0] << "  " << argv[1] << "\n";
    if (execve(path.c_str(), argv, env_str) == -1)
        perror("execve");
    exit(EXIT_FAILURE);
}

char **Executor::_get_env(std::map<std::string, std::string> &env) {
    for (uint32_t i = 0; i < env_arr_length; ++i) {
        if (env.find(env_string[i]) == env.end()) {
            std::string key = env_string[i];
            std::string val;
            env.insert(std::make_pair<std::string, std::string>(key, val));
        }
    }

    char   **ret = new char *[env.size() + 1];
    uint32_t i = 0;
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
        ret[i] = new char[it->first.length() + it->second.length() + 2];
        uint32_t j = 0;
        for (uint32_t k = 0; k < it->first.length(); ++k, ++j) {
            ret[i][j] = it->first[k];
        }
        ret[i][j] = '=';
        ++j;
        for (uint32_t k = 0; k < it->second.length(); ++k, ++j) {
            ret[i][j] = it->second[k];
        }
        ret[i][j] = '\0';
        ++i;
    }
    ret[env.size()] = NULL;

    return ret;
}

char **Executor::_get_argv(const std::string &path, const std::string &body) {
    char **argv = new char *[3];
    argv[0] = new char[path.size() + 1];
    for (std::size_t i = 0; i < path.size(); ++i) {
        argv[0][i] = path[i];
    }
    argv[0][path.size()] = '\0';
    argv[1] = new char[body.size() + 1];
    for (std::size_t i = 0; i < body.size(); ++i) {
        argv[1][i] = body[i];
    }
    argv[1][body.size()] = '\0';
    argv[2] = NULL;
    return argv;
}

}  // namespace cgi