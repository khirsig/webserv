/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:20:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/07 11:11:52 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"

static const int         env_arr_length = 19;
static const std::string env_string[env_arr_length] = {
    "auth_type",       "content_length", "content_type",    "gateway_interface", "http_accept",
    "http_user_agent", "path_info",      "path_translated", "query_string",      "remote_addr",
    "remote_host",     "remote_ident",   "remote_user",     "request_method",    "script_name",
    "server_name",     "server_port",    "server_protocol", "server_software"};

namespace cgi {

class Handler {
   public:
    Handler();
    ~Handler();

    void    execute(core::ByteBuffer &path, core::ByteBuffer &body,
                    std::map<std::string, std::string> &env);
    int32_t get_fd() const;

   private:
    int32_t _read_fd;
    pid_t   _id;

    void   _run_program(core::ByteBuffer &path, core::ByteBuffer &body,
                        std::map<std::string, std::string> &env, int fd[2]);
    char **_get_env(std::map<std::string, &std::string> env);
};
}  // namespace cgi
