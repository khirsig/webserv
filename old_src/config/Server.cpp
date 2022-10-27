/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:14 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/18 15:04:04 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace config {
bool Listen::operator==(const Listen &rhs) { return addr == rhs.addr && port == rhs.port; }

void Server::print() const {
    for (std::vector<Listen>::const_iterator it = v_listen.begin(); it != v_listen.end(); ++it) {
        std::cout << "listen\n{\n";
        std::cout << "    address: \"" << it->addr << "\"\n";
        std::cout << "    port: \"" << htons(it->port) << "\"\n";
        std::cout << "}\n";
    }
    for (std::vector<std::string>::const_iterator it = v_server_name.begin();
         it != v_server_name.end(); ++it) {
        std::cout << "server_name: " << *it << "\n";
    }
    for (std::vector<ErrorPage>::const_iterator it = v_error_page.begin(); it != v_error_page.end();
         ++it) {
        std::cout << "error_page\n{\n";
        std::cout << "    status:";
        for (std::vector<std::uint32_t>::const_iterator iter = it->v_code.begin();
             iter != it->v_code.end(); ++iter) {
            std::cout << " \"" << *iter << "\"";
        }
        std::cout << "\n    path: \"" << it->path << "\"\n";
        std::cout << "}\n";
    }
    std::cout << "client_max_body_size: " << client_max_body_size << "\n";
    for (std::vector<Location>::const_iterator it = v_location.begin(); it != v_location.end();
         ++it) {
        std::cout << "Location:\n";
        std::cout << "{\n";
        it->print("    ");
        std::cout << "}\n";
    }
}

}  // namespace config