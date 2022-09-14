#include "Eni.hpp"

#include <cerrno>
#include <stdexcept>

Eni::Eni() {
    _kq = kqueue();
    if (_kq == -1) {
        throw std::runtime_error("kqueue: " + std::string(strerror(errno)));
    }
    events = new struct kevent[MAX_POLLED_EVENTS];
}

Eni::~Eni() {
    delete[] events;
    close(_kq);
}

int Eni::add_event(int fd, int16_t filter, int64_t data) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_ADD, 0, data, NULL);
    return kevent(_kq, &event, 1, NULL, 0, NULL);
}

int Eni::delete_event(int fd, int16_t filter) {
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    return kevent(_kq, &event, 1, NULL, 0, NULL);
}

int Eni::poll_events() { return kevent(_kq, NULL, 0, events, MAX_POLLED_EVENTS, NULL); }
