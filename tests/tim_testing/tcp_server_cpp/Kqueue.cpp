#include "Kqueue.hpp"

#include <cerrno>
#include <stdexcept>

Kqueue::Kqueue() {
    _kq = kqueue();
    if (_kq == -1) {
        throw std::runtime_error("kqueue" + std::string(strerror(errno)));
    }
    events = new struct kevent[MAX_POLLED_EVENTS];
}

Kqueue::~Kqueue() {
    delete[] events;
    close(_kq);
}

int Kqueue::add_event(int fd, int filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_ADD, 0, 0, NULL);
    return kevent(_kq, &event, 1, NULL, 0, NULL);
}

int Kqueue::delete_event(int fd, int filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    return kevent(_kq, &event, 1, NULL, 0, NULL);
}

int Kqueue::poll_events() { return kevent(_kq, NULL, 0, events, MAX_POLLED_EVENTS, NULL); }
