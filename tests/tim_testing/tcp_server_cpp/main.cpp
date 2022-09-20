#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "Connections.hpp"
#include "EventNotificationInterface.hpp"
#include "Socket.hpp"

void recv_msg(int fd, const Connections& con) {
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    std::cerr << "bytes_read: " << bytes_read << '\n';
    buf[bytes_read] = 0;
    std::cout << con.get_connection_ip(fd) << ":" << con.get_connection_port(fd) << " # " << buf;
}

int main(int argc, char* argv[]) {
    EventNotificationInterface eni;
    std::vector<Socket>        v_socket;

    v_socket.push_back(Socket(inet_addr("127.0.0.1"), htons(8080)));
    v_socket.push_back(Socket(inet_addr("10.11.2.12"), htons(8080)));

    for (std::vector<Socket>::iterator it = v_socket.begin(); it != v_socket.end(); ++it) {
        eni.add_event(it->fd, EVFILT_READ, 0);
    }

    Connections connections(1);
    while (42) {
        int num_events = eni.poll_events();
        if (num_events == -1) {
            std::cerr << "poll_events: " << strerror(errno) << '\n';
            continue;
        }
        for (int i = 0; i < num_events; i++) {
            if (eni.events[i].flags & EV_ERROR) {
                std::cerr << "kevent() error on " << eni.events[i].ident << '\n';
            } else if (std::find(v_socket.begin(), v_socket.end(), eni.events[i].ident) !=
                       v_socket.end()) {
                int fd = connections.accept_connection(eni.events[i].ident, eni);
                if (fd != -1)
                    std::cerr << "Accept new connection: " << connections.get_connection_ip(fd)
                              << ":" << connections.get_connection_port(fd) << '\n';
            } else if (eni.events[i].flags & EV_EOF || eni.events[i].filter == EVFILT_TIMER) {
                if (eni.events[i].filter == EVFILT_TIMER) {
                    std::cerr << "Timeout on connection: "
                              << connections.get_connection_ip(eni.events[i].ident) << ":"
                              << connections.get_connection_port(eni.events[i].ident) << '\n';
                } else if (eni.events[i].flags & EV_EOF) {
                    recv_msg(eni.events[i].ident, connections);
                    eni.add_event(eni.events[i].ident, EVFILT_TIMER, CONNECTION_TIMEOUT);
                    write(eni.events[i].ident, "response\n", 9);
                }
                std::cerr << "Closed connection: "
                          << connections.get_connection_ip(eni.events[i].ident) << ":"
                          << connections.get_connection_port(eni.events[i].ident) << '\n';
                connections.close_connection(eni.events[i].ident, eni);
            } else if (eni.events[i].filter == EVFILT_READ) {
                recv_msg(eni.events[i].ident, connections);
                eni.add_event(eni.events[i].ident, EVFILT_TIMER, CONNECTION_TIMEOUT);
            }
        }
    }
    return EXIT_SUCCESS;
}
