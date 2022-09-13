#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "Connections.hpp"
#include "Kqueue.hpp"
#include "Socket.hpp"

void recv_msg(int fd, const Connections& con) {
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    buf[bytes_read] = 0;
    std::cout << con.get_connection_ip(fd) << ":" << con.get_connection_port(fd) << " # " << buf;
}

int main(int argc, char* argv[]) {
    Kqueue              kq;
    std::vector<Socket> v_socket;

    v_socket.push_back(Socket(inet_addr("127.0.0.1"), 8080));
    v_socket.push_back(Socket(inet_addr("10.11.2.12"), 8080));

    for (std::vector<Socket>::iterator it = v_socket.begin(); it != v_socket.end(); ++it) {
        kq.add_event((*it).fd, EVFILT_READ);
    }

    Connections connections(1024);
    while (42) {
        int num_events = kq.poll_events();
        for (int i = 0; i < num_events; i++) {
            if (kq.events[i].flags & EV_ERROR) {
                std::cerr << "kevent() error on " << kq.events[i].ident << '\n';
            } else if (std::find(v_socket.begin(), v_socket.end(), kq.events[i].ident) !=
                       v_socket.end()) {
                connections.accept_connection(kq.events[i].ident, kq);
            } else if (kq.events[i].flags & EV_EOF) {
                connections.close_connection(kq.events[i].ident, kq);
            } else if (kq.events[i].filter == EVFILT_READ) {
                recv_msg(kq.events[i].ident, connections);
            }
        }
    }
    return EXIT_SUCCESS;
}
