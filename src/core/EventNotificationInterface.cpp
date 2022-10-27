#include "EventNotificationInterface.hpp"

#include <cerrno>
#include <stdexcept>

namespace core {

EventNotificationInterface::EventNotificationInterface() {
    _kq_fd = kqueue();
    if (_kq_fd == -1) {
        throw std::runtime_error("kqueue: " + std::string(strerror(errno)));
    }
    events = new struct kevent[MAX_POLLED_EVENTS];
}

EventNotificationInterface::~EventNotificationInterface() {
    delete[] events;
    close(_kq_fd);
}

int EventNotificationInterface::add_event(int fd, int16_t filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_ADD, 0, 0, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
}

int EventNotificationInterface::add_timer(int fd, ssize_t ms) {
    struct kevent event;
    EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, ms, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
}

int EventNotificationInterface::delete_event(int fd, int16_t filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
}

int EventNotificationInterface::poll_events() {
    return kevent(_kq_fd, NULL, 0, events, MAX_POLLED_EVENTS, NULL);
}

int EventNotificationInterface::enable_event(int fd, int16_t filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_ENABLE, 0, 0, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
}

int EventNotificationInterface::disable_event(int fd, int16_t filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DISABLE, 0, 0, NULL);
    return kevent(_kq_fd, &event, 1, NULL, 0, NULL);
}

}  // namespace core
