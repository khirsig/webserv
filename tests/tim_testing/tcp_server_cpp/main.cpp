#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "Connections.hpp"
#include "Eni.hpp"
#include "Socket.hpp"

void recv_msg(int fd, const Connections& con) {
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    buf[bytes_read] = 0;
    std::cout << con.get_connection_ip(fd) << ":" << con.get_connection_port(fd) << " # " << buf;
}

int main(int argc, char* argv[]) {
    Eni                 eni;
    std::vector<Socket> v_socket;

    v_socket.push_back(Socket(inet_addr("127.0.0.1"), 8080));
    v_socket.push_back(Socket(inet_addr("10.11.2.12"), 8080));

    for (std::vector<Socket>::iterator it = v_socket.begin(); it != v_socket.end(); ++it) {
        eni.add_event(it->fd, EVFILT_READ);
    }

    Connections connections(1024);
    while (42) {
        int num_events = eni.poll_events();
        for (int i = 0; i < num_events; i++) {
            if (eni.events[i].flags & EV_ERROR) {
                std::cerr << "kevent() error on " << eni.events[i].ident << '\n';
            } else if (std::find(v_socket.begin(), v_socket.end(), eni.events[i].ident) !=
                       v_socket.end()) {
                connections.accept_connection(eni.events[i].ident, eni);
            } else if (eni.events[i].flags & EV_EOF) {
                connections.close_connection(eni.events[i].ident, eni);
            } else if (eni.events[i].filter == EVFILT_READ) {
                recv_msg(eni.events[i].ident, connections);
            }
        }
    }
    return EXIT_SUCCESS;
}
