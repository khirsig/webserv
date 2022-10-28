#pragma once

#include <vector>

#include "../config/Server.hpp"
#include "Connection.hpp"
#include "EventNotificationInterface.hpp"

#define MAX_PIPE_SIZE 1048576
#define TIMEOUT_TIME 6000

namespace core {
class Webserver {
   private:
    std::vector<Connection>            _v_connection;
    EventNotificationInterface         _eni;
    const std::vector<config::Server> &_v_server;
    char                              *_read_buf;
    const size_t                       _read_buf_size;
    size_t                             _used_connections;
    const size_t                       _max_connections;

   public:
    Webserver(std::vector<config::Server> v_server, size_t max_connections);
    ~Webserver();

    void accept_connection(const Socket &socket);
    void close_connection(int fd);
    void close_connection(std::vector<Connection>::iterator it);
    void timeout_connection(int fd);

    void receive(int fd, size_t data_len);
    void send(int fd, size_t max_len);
};

}  // namespace core