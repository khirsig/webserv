#pragma once

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "ByteBuffer.hpp"
#include "EventNotificationInterface.hpp"

#define CGI_READ_BUFFER_SIZE 1024

static const int         env_arr_length = 19;
static const std::string env_string[env_arr_length] = {
    "AUTH_TYPE",       "CONTENT_LENGTH",  "CONTENT_TYPE",   "GATEWAY_INTERFACE", "HTTP_ACCEPT",
    "HTTP_USER_AGENT", "PATH_INFO",       "QUERY_STRING",   "REMOTE_ADDR",       "REMOTE_HOST",
    "REMOTE_IDENT",    "REMOTE_USER",     "REQUEST_METHOD", "SCRIPT_NAME",       "SERVER_NAME",
    "SERVER_PORT",     "SERVER_PROTOCOL", "SERVER_SOFTWARE"};

namespace core {

class EventNotificationInterface;

class CgiHandler {
   public:
    CgiHandler(const http::Request &request, http::Response &response);
    ~CgiHandler();

    void init(int connection_fd);
    void execute(EventNotificationInterface &eni, const std::string &cgi_path);
    void reset();
    void read(EventNotificationInterface &eni, bool eof);
    void write(EventNotificationInterface &eni, std::size_t max_size);

    bool is_done() const;

    int get_read_fd() const;
    int get_write_fd() const;

   private:
    const http::Request &_request;
    http::Response      &_response;

    int    _read_fd;
    int    _write_fd;
    int    _connection_fd;
    pid_t  _pid;
    bool   _is_done;
    size_t _body_pos;

    void   _run_program(const std::string &cgi_path);
    char **_get_env(std::map<std::string, std::string> &env);
    void   _update_env(std::map<std::string, std::string> &env);
    char **_get_argv(const std::string &path, const std::string &body);
};

extern std::map<int, CgiHandler *> g_executor;

}  // namespace core
