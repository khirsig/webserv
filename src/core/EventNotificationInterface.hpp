#pragma once

#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <vector>

#include "CgiHandler.hpp"
#include "Socket.hpp"

#define MAX_POLLED_EVENTS 16

namespace core {

class CgiHandler;

class EventNotificationInterface {
   private:
    int                              _kq_fd;
    std::map<int, const CgiHandler&> _m_cgi;
    const std::map<int, Socket>&     _m_socket;

   public:
    struct kevent* events;

    EventNotificationInterface(const std::map<int, Socket>& m_socket);
    ~EventNotificationInterface();

    int add_event(int fd, int16_t filter);
    int add_timer(int fd, ssize_t ms);
    int delete_event(int fd, int16_t filter);
    int enable_event(int fd, int16_t filter);
    int disable_event(int fd, int16_t filter);
    int poll_events();

    const Socket*     find_socket(int fd);
    const CgiHandler* find_cgi(int fd);
    void              add_cgi_fd(int fd, const CgiHandler& cgi);
    void              remove_cgi_fd(int fd);
};

}  // namespace core
