/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/27 10:00:55 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>

#include "Location.hpp"

namespace config {

class Address {
   public:
    in_addr_t addr;
    in_port_t port;

    bool operator==(const Address &rhs);
};

class ErrorPage {
   public:
    std::string                path;
    std::vector<std::uint32_t> v_code;
};

class Server {
   public:
    void print() const;

    std::vector<Address>     v_listen;
    std::vector<std::string> v_server_name;
    std::vector<ErrorPage>   v_error_page;
    std::uint64_t            client_max_body_size;
    std::vector<Location>    v_location;
};

}  // namespace config