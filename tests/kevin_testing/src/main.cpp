/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:04:24 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/15 10:53:10 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Parser.hpp"

int main() {
    std::string file_path = "./webserv.conf";

    config::Parser              parser;
    std::vector<config::Server> v_server;

    parser.parse(file_path, v_server);

    // for (std::vector<config::Token>::iterator it = v_token.begin(); it != v_token.end();
    // ++it) {
    //     std::cout << "Text: \"" << it->text << "\" | Type: " <<
    //     config::token_type_string[it->type]
    //               << "\n";
    // }

    std::cout << "\n\n";

    for (std::vector<config::Server>::iterator it = v_server.begin(); it != v_server.end(); ++it) {
        it->print();
        std::cout << "\n\n";
    }
    return 0;
}