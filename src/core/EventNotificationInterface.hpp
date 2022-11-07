#pragma once

#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <vector>

#define MAX_POLLED_EVENTS 16

namespace core {

class EventNotificationInterface {
   private:
    int _kq_fd;
    // std::map<int, const core::CgiHandler &> _m_cgi;
    // std::map<int, const core::Server &> _m_cgi;

   public:
    struct kevent* events;

    EventNotificationInterface();
    ~EventNotificationInterface();

    int add_event(int fd, int16_t filter);
    int add_timer(int fd, ssize_t ms);
    int delete_event(int fd, int16_t filter);
    int enable_event(int fd, int16_t filter);
    int disable_event(int fd, int16_t filter);
    int poll_events();

    // bool is_server_fd(int fd);
    // bool is_cgi_fd(int fd);
    // void add_cgi_fd(int fd, const config::CgiHandler& cgi);
    // void remove_cgi_fd(int fd);
};

}  // namespace core
