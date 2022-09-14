/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/14 13:40:26 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Location.hpp"

namespace config {

class Server {
   public:
    void print() const;

    std::vector<std::string> v_listen;
    std::vector<std::string> v_server_name;
    std::vector<std::string> v_error_page;
    std::uint64_t            client_max_body_size;
    std::vector<Location>    v_location;
};

}  // namespace config