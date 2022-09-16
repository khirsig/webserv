#include "EventNotificationInterface.hpp"

#include <cerrno>
#include <stdexcept>

EventNotificationInterface::EventNotificationInterface() {
#if defined(KQUEUE)
    _kq_fd = kqueue();
    if (_kq_fd == -1) {
        throw std::runtime_error("kqueue: " + std::string(strerror(errno)));
    }
    events = new struct kevent[MAX_POLLED_EVENTS];
#elif defined(EPOLL)
    _epoll_fd = epoll_create1(0);
    if (_kq_fd == -1) {
        throw std::runtime_error("epoll_create1: " + std::string(strerror(errno)));
    }
    events = new struct epoll_event[MAX_POLLED_EVENTS];
#endif
}

EventNotificationInterface::~EventNotificationInterface() {
#if defined(KQUEUE)
    delete[] events;
    close(_kq_fd);
#elif defined(EPOLL)
    delete[] events;
    close(_epoll_fd);
#endif
}

int EventNotificationInterface::add_event(int fd, int16_t filter, int64_t data) {
#if defined(KQUEUE)
    struct kevent event;
    EV_SET(&event, fd, filter, EV_ADD, 0, data, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
#elif defined(EPOLL)
    struct epoll_event event;
    event.data.fd = fd;
    event.events = filter;
    return epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event);
#endif
}

int EventNotificationInterface::delete_event(int fd, int16_t filter) {
#if defined(KQUEUE)
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
#elif defined(EPOLL)

#endif
}

int EventNotificationInterface::poll_events() {
#if defined(KQUEUE)
    return kevent(_kq_fd, NULL, 0, events, MAX_POLLED_EVENTS, NULL);
#elif defined(EPOLL)
    return epoll_wait(_epoll_fd, events, MAX_POLLED_EVENTS, -1);
#endif
}
