/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:20:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/24 15:41:24 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../core/ByteBuffer.hpp"
#include "../core/EventNotificationInterface.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"

#define CGI_READ_BUFFER_SIZE 1024

static const int         env_arr_length = 19;
static const std::string env_string[env_arr_length] = {
    "AUTH_TYPE",   "CONTENT_LENGTH",  "CONTENT_TYPE",    "GATEWAY_INTERFACE",
    "HTTP_ACCEPT", "HTTP_USER_AGENT", "PATH_INFO",       "REMOTE_ADDR",
    "REMOTE_HOST", "REMOTE_IDENT",    "REMOTE_USER",     "SCRIPT_NAME",
    "SERVER_NAME", "SERVER_PORT",     "SERVER_PROTOCOL", "SERVER_SOFTWARE"};

namespace http {
class Response;
}

namespace cgi {

class Executor {
   public:
    Executor(http::Request &request, http::Response &response,
             core::EventNotificationInterface &eni);
    ~Executor();

    void execute(std::string &root, std::string &path);
    void read(bool eof);
    void write(std::size_t max_size);

    int get_read_fd() const;
    int get_write_fd() const;

   private:
    int   _read_fd;
    int   _write_fd;
    pid_t _id;

    http::Request                    &_request;
    http::Response                   &_response;
    core::EventNotificationInterface &_eni;

    std::size_t _request_index;

    void   _run_program(std::string &root, std::string &path);
    char **_get_env(std::map<std::string, std::string> &env);
    void   _update_env(std::map<std::string, std::string> &env);
    char **_get_argv(const std::string &path, const std::string &body);
};

extern std::map<int, Executor *> g_executor;

}  // namespace cgi
