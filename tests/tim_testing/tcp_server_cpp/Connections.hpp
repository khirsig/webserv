#pragma once

#include <vector>

#include "Eni.hpp"

class Connections {
   private:
    std::vector<int>                _v_fd;
    std::vector<struct sockaddr_in> _v_address;
    std::vector<socklen_t>          _v_address_len;
    size_t                          _max_connections;

    int get_index(int fd) const;

   public:
    Connections(size_t max_connections);
    ~Connections();

    int         accept_connection(int fd, Eni& kq);
    int         close_connection(int fd, Eni& kq);
    std::string get_connection_ip(int fd) const;
    int         get_connection_port(int fd) const;
};
