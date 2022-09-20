/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 09:56:29 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/20 10:30:28 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "config/Parser.hpp"
#include "core/Connections.hpp"
#include "core/EventNotificationInterface.hpp"
#include "core/Socket.hpp"

#define DEBUG

void recv_msg(int fd, const core::Connections& con) {
    char buf[1024];
    int  bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    std::cerr << "bytes_read: " << bytes_read << '\n';
    buf[bytes_read] = 0;
    std::cout << con.get_connection_ip(fd) << ":" << con.get_connection_port(fd) << " # " << buf;
}

int main(int argc, char* argv[]) {
    std::string file_path;
    if (argc == 1)
        file_path = "./webserv.conf";
    else if (argc == 2)
        file_path = argv[1];
    else
        return 1;

    config::Parser              parser;
    std::vector<config::Server> v_server;

    parser.parse(file_path, v_server);

#ifdef DEBUG
    std::cout << "\n\n";

    for (std::vector<config::Server>::iterator it = v_server.begin(); it != v_server.end(); ++it) {
        it->print();
        std::cout << "\n\n";
    }
#endif

    core::EventNotificationInterface eni;
    std::vector<core::Socket>        v_socket;

    for (std::vector<config::Server>::iterator it_server = v_server.begin();
         it_server != v_server.end(); ++it_server) {
        for (std::vector<config::Listen>::iterator it_listen = it_server->v_listen.begin();
             it_listen != it_server->v_listen.end(); ++it_listen) {
            v_socket.push_back(core::Socket(it_listen->addr, it_listen->port));
        }
    }

    for (std::vector<core::Socket>::iterator it = v_socket.begin(); it != v_socket.end(); ++it) {
        eni.add_event(it->fd, EVFILT_READ, 0);
    }

    core::Connections connections(1);
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

    return 0;
}