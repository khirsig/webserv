/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:04:24 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 13:43:16 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <sstream>

#include "Parser.hpp"
#include "Server.hpp"
#include "Tokenizer.hpp"

int main() {
    std::string       file_path = "./test.conf";
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    std::cout << "PARSER\n\n";
    file.open(file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file \"" << file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    ft::Tokenizer          tokenizer;
    std::vector<ft::Token> v_token = tokenizer.parse(file_content);

    // for (ft::Token currToken : v_token) {
    //     currToken.debug_print();
    // }

    std::vector<ft::Server> server;
    ft::Parser              parser(file_path);

    parser.parse(v_token, server);
    for (std::vector<ft::Server>::iterator it = server.begin(); it != server.end(); ++it) {
        it->print();
        std::cout << "\n\n";
    }
    return 0;
}