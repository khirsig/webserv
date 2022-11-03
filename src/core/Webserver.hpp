#pragma once

#include <vector>

#include "../config/Server.hpp"
#include "../settings.hpp"
#include "Connection.hpp"
#include "EventNotificationInterface.hpp"
#include "Socket.hpp"

namespace core {

class Webserver {
   private:
    std::vector<Connection>            _v_connection;
    EventNotificationInterface         _eni;
    const std::vector<config::Server> &_v_server;
    size_t                             _used_connections;
    const size_t                       _max_connections;
    std::map<int, Socket>              _m_socket;

    void _accept_connection(const Socket &socket);
    void _close_connection(int fd);
    void _close_connection(std::vector<Connection>::iterator it);
    void _timeout_connection(int fd);

    void _receive(int fd, size_t data_len);
    void _send(int fd, size_t max_len);

   public:
    Webserver(const std::vector<config::Server> &v_server, size_t max_connections);
    ~Webserver();

    void run();
};

}  // namespace core
