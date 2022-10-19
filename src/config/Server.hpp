/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/18 15:03:29 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>

#include "Location.hpp"

namespace config {

class Listen {
   public:
    in_addr_t addr;
    in_port_t port;

    bool operator==(const Listen &rhs);
};

class ErrorPage {
   public:
    std::string                path;
    std::vector<std::uint32_t> v_code;
};

class Server {
   public:
    void print() const;

    std::vector<Listen>      v_listen;
    std::vector<std::string> v_server_name;
    std::vector<ErrorPage>   v_error_page;
    std::uint64_t            client_max_body_size;
    std::vector<Location>    v_location;
};

}  // namespace config