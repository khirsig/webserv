/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:04:24 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/14 13:26:17 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <sstream>

#include "Parser.hpp"
#include "Server.hpp"
#include "Tokenizer.hpp"

int main() {
    std::string       file_path = "./webserv.conf";
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    file.open(file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file \"" << file_path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    config::Tokenizer          tokenizer;
    std::vector<config::Token> v_token = tokenizer.parse(file_content);
    std::cout << "\n\n";

    std::vector<config::Server> server;
    config::Parser              parser(file_path);

    parser.parse(v_token, server);
    for (std::vector<config::Server>::iterator it = server.begin(); it != server.end(); ++it) {
        it->print();
        std::cout << "\n\n";
    }
    return 0;
}