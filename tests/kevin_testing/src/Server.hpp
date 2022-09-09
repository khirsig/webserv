/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 09:34:22 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 10:54:06 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace ft {

class Server {
   public:
    void print() const;

    std::vector<std::string> &get_v_listen() { return _v_listen; }
    std::vector<std::string> &get_v_server_name() { return _v_server_name; }
    std::vector<std::string> &get_v_error_page() { return _v_error_page; }
    std::string              &get_client_max_body_size() { return _client_max_body_size; }

   private:
    std::vector<std::string> _v_listen;
    std::vector<std::string> _v_server_name;
    std::vector<std::string> _v_error_page;
    std::string              _client_max_body_size;
};

}  // namespace ft