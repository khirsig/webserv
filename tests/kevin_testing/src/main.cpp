/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:04:24 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/20 09:57:26 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Parser.hpp"

int main() {
    std::string file_path = "./webserv.conf";

    config::Parser              parser;
    std::vector<config::Server> v_server;

    parser.parse(file_path, v_server);

    std::cout << "\n\n";

    for (std::vector<config::Server>::iterator it = v_server.begin(); it != v_server.end(); ++it) {
        it->print();
        std::cout << "\n\n";
    }
    return 0;
}