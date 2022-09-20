#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <vector>

#define KQUEUE

#if defined(KQUEUE)
#include <sys/event.h>
#elif defined(EPOLL)
#include <sys/epoll.h>
#endif

#define MAX_POLLED_EVENTS 16

namespace core {

class EventNotificationInterface {
   private:
#if defined(KQUEUE)
    int _kq_fd;
#elif defined(EPOLL)
    int                 _epoll_fd;
#endif

   public:
#if defined(KQUEUE)
    struct kevent *events;
#elif defined(EPOLL)
    struct epoll_event *events;
#endif

    EventNotificationInterface();
    ~EventNotificationInterface();

    int add_event(int fd, int16_t filter, int64_t data);
    int delete_event(int fd, int16_t filter);
    int poll_events();
};

}  // namespace core
