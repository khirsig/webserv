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

   public:
    struct kevent *events;

    EventNotificationInterface();
    ~EventNotificationInterface();

    int add_event(int fd, int16_t filter, int64_t data);
    int delete_event(int fd, int16_t filter);
    int enable_event(int fd, int16_t filter);
    int disable_event(int fd, int16_t filter);
    int poll_events();
};

}  // namespace core
