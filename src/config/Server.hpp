/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/11/03 13:03:43 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../core/Address.hpp"
#include "Location.hpp"

namespace config {

class ErrorPage {
   public:
    std::string                path;
    std::vector<std::uint32_t> v_code;
};

class Server {
   public:
    void print() const;

    std::vector<core::Address> v_listen;
    std::vector<std::string>   v_server_name;
    std::vector<ErrorPage>     v_error_page;
    std::uint64_t              client_max_body_size;
    std::vector<Location>      v_location;
};

}  // namespace config