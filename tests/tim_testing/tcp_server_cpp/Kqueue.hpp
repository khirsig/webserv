#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <vector>

#ifdef LINUX
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

#define MAX_POLLED_EVENTS 16

class Kqueue {
   private:
    int _kq;

   public:
    struct kevent *events;

    Kqueue();
    ~Kqueue();

    int add_event(int fd, int filter);
    int delete_event(int fd, int filter);
    int poll_events();
};
