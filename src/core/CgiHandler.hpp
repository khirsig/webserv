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

namespace core {

class EventNotificationInterface;

class CgiHandler {
   private:
    const http::Request &_request;
    http::Response      &_response;

    int    _read_fd;
    int    _write_fd;
    int    _connection_fd;
    pid_t  _pid;
    bool   _is_done;
    size_t _body_pos;
    char  *_buf;

    void   _run_program(const std::string &cgi_path, const std::string &script_path);
    char **_get_env(std::map<std::string, std::string> &env);
    void   _update_env(std::map<std::string, std::string> &env);
    char **_get_argv(const std::string &path, const std::string &body);

   public:
    CgiHandler(const http::Request &request, http::Response &response);
    ~CgiHandler();

    void init(int connection_fd);
    void execute(EventNotificationInterface &eni, const std::string &cgi_path,
                 const std::string &script_path);
    void reset(EventNotificationInterface &eni);
    void stop(EventNotificationInterface &eni);
    void eof_read(EventNotificationInterface &eni);
    void read(EventNotificationInterface &eni, size_t data_len);
    void eof_write(EventNotificationInterface &eni);
    void write(EventNotificationInterface &eni, std::size_t max_size);

    bool is_done() const;

    int get_read_fd() const;
    int get_write_fd() const;
};

extern std::map<int, CgiHandler *> g_executor;

}  // namespace core
