#pragma once

#include <vector>

#include "../config/Server.hpp"
#include "Connection.hpp"
#include "EventNotificationInterface.hpp"

#define MAX_PIPE_SIZE 1048576

namespace core {

class ConnectionHandler {
   private:
    std::vector<Connection>            _v_connection;
    EventNotificationInterface         _eni;
    const std::vector<config::Server> &_v_server;
    char                              *_read_buf;
    const size_t                       _read_buf_size;
    size_t                             _used_connections;
    size_t                             _max_connections;

   public:
    ConnectionHandler(std::vector<config::Server> v_server, size_t max_connections);
    ~ConnectionHandler();

    void accept(int fd);
    void receive(int fd);
    void send(int fd);
    void close(int fd);
    void timeout(int fd);
};

}  // namespace core