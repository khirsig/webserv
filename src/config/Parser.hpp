/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/15 10:38:15 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/15 10:49:13 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "Interpreter.hpp"
#include "Tokenizer.hpp"

namespace config {

class Parser {
   public:
    void parse(const std::string &file_path, std::vector<Server> &v_server);

   private:
    std::string _file_to_string(std::string file_path);
};

}  // namespace config
