/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 15:14:32 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Location.hpp"

namespace ft {

class Server {
   public:
    void print() const;

    std::vector<std::string> v_listen;
    std::vector<std::string> v_server_name;
    std::vector<std::string> v_error_page;
    std::string              client_max_body_size;
    std::vector<Location>    v_location;
};

}  // namespace ft