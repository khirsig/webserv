/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:14 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 11:04:45 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace ft {

void Server::print() const {
    for (std::vector<std::string>::const_iterator it = _v_listen.begin(); it != _v_listen.end();
         ++it) {
        std::cout << "listen: " << *it << "\n";
    }
    for (std::vector<std::string>::const_iterator it = _v_server_name.begin();
         it != _v_server_name.end(); ++it) {
        std::cout << "server_name: " << *it << "\n";
    }
    for (std::vector<std::string>::const_iterator it = _v_error_page.begin();
         it != _v_error_page.end(); ++it) {
        std::cout << "error_page: " << *it << "\n";
    }
    std::cout << "client_max_body_size: " << _client_max_body_size << "\n";
}

}  // namespace ft