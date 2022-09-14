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
#else
#include <poll.h>
#endif

#define MAX_POLLED_EVENTS 16

// Event Notification Interface
class Eni {
   private:
#if defined(KQUEUE)
    int _kq;
#elif defined(EPOLL)

#else
    struct pollfd *_poll_fds;
#endif

   public:
    struct kevent *events;

    Eni();
    ~Eni();

    int add_event(int fd, int16_t filter, int64_t data);
    int delete_event(int fd, int16_t filter);
    int poll_events();
};
