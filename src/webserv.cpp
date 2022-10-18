/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 09:56:29 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/18 13:21:32 by tjensen          ###   ########.fr       */
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
#include "http/StatusCode.hpp"
#include "http/httpStatusCodes.hpp"
#include "log/Log.hpp"

http::StatusCode status_code;
// #define DEBUG_CONFIG_PARSER

int main(int argc, char* argv[]) {
    try {
        std::string file_path;
        if (argc == 1)
            file_path = "./webserv.conf";
        else if (argc == 2)
            file_path = argv[1];
        else {
            std::cerr << "Error\nusage: ./build/webserv [config_file]\n";
            return EXIT_FAILURE;
        }

        status_code.init();

        config::Parser              parser;
        std::vector<config::Server> v_server;

        parser.parse(file_path, v_server);

        core::EventNotificationInterface eni;
        std::map<int, core::Socket>      m_socket;
        // std::vector<core::Socket>        v_socket;

        // Create Socket for each listen in config file
        // Add read event for each socket fd
        for (std::vector<config::Server>::iterator it_server = v_server.begin();
             it_server != v_server.end(); ++it_server) {
            for (std::vector<config::Listen>::iterator it_listen = it_server->v_listen.begin();
                 it_listen != it_server->v_listen.end(); ++it_listen) {
                core::Socket socket(it_listen->addr, it_listen->port);
                // v_socket.push_back(socket);
                m_socket.insert(std::make_pair(socket.getFD(), socket));
                eni.add_event(socket.getFD(), EVFILT_READ, 0);
            }
        }

        core::Connections connections(1024, v_server);
        while (42) {
            try {
                int num_events = eni.poll_events();
                if (num_events == -1)
                    throw std::runtime_error("poll_events: " + std::string(strerror(errno)));
                for (int i = 0; i < num_events; i++) {
                    if (eni.events[i].flags & EV_ERROR) {
                        throw std::runtime_error("kevent: " + std::string(strerror(errno)));
                    } else if (m_socket.find(eni.events[i].ident) != m_socket.end()) {
                        connections.accept_connection(eni.events[i].ident, eni);
                    } else if (eni.events[i].filter == EVFILT_TIMER) {
                        connections.timeout_connection(eni.events[i].ident, eni);
                    } else if (eni.events[i].flags & EV_EOF) {
                        if (eni.events[i].filter == EVFILT_READ) {
                            int index = connections.recv_request(eni.events[i].ident, eni);
                            connections.parse_request(index, eni);
                        }
                        connections.close_connection(eni.events[i].ident, eni);
                    } else if (eni.events[i].filter == EVFILT_WRITE) {
                        connections.send_response(eni.events[i].ident, eni, eni.events[i].data);
                    } else if (eni.events[i].filter == EVFILT_READ) {
                        int index = connections.recv_request(eni.events[i].ident, eni);
                        connections.parse_request(index, eni);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << core::timestamp() << e.what() << '\n';
            } catch (...) {
                std::cerr << core::timestamp() << "Unknown error\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << core::timestamp() << "" << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
