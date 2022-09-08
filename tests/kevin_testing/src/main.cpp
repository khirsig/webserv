/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 16:04:24 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/08 15:01:39 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <sstream>

#include "Tokenizer.hpp"

int main() {
    std::string       path("./webserv.conf");
    std::ifstream     file;
    std::stringstream buf;
    std::string       file_content;

    std::cout << "Tokenizer\n\n";
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file \"" << path << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    buf << file.rdbuf();
    file_content = buf.str();

    ft::Tokenizer          tokenizer;
    std::vector<ft::Token> v_token = tokenizer.parse(file_content);

    for (ft::Token currToken : v_token) {
        currToken.debug_print();
    }
    return 0;
}