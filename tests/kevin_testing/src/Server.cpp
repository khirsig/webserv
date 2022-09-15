/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:14 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/15 14:34:00 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace config {

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
    for (std::vector<std::string>::const_iterator it = v_error_page.begin();
         it != v_error_page.end(); ++it) {
        std::cout << "error_page: " << *it << "\n";
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