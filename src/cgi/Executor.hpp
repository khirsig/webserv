/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:20:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/24 10:09:26 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"
#include "../http/Request.hpp"

static const int         env_arr_length = 19;
static const std::string env_string[env_arr_length] = {
    "AUTH_TYPE",       "CONTENT_LENGTH", "CONTENT_TYPE",    "GATEWAY_INTERFACE", "HTTP_ACCEPT",
    "HTTP_USER_AGENT", "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING",      "REMOTE_ADDR",
    "REMOTE_HOST",     "REMOTE_IDENT",   "REMOTE_USER",     "REQUEST_METHOD",    "SCRIPT_NAME",
    "SERVER_NAME",     "SERVER_PORT",    "SERVER_PROTOCOL", "SERVER_SOFTWARE"};

namespace cgi {

class Executor {
   public:
    Executor();
    ~Executor();

    void    execute(http::Request &request, std::string &root, std::string &path);
    int32_t get_fd() const;

   private:
    int32_t _read_fd;
    pid_t   _id;

    void   _run_program(http::Request &request, std::string &root, std::string &path, int fd[2]);
    char **_get_env(http::Request &request, std::map<std::string, std::string> &env);
    void   _update_env(http::Request &request, std::map<std::string, std::string> &env);
    char **_get_argv(const std::string &path, const std::string &body);
};

}  // namespace cgi