/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 14:10:30 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/20 16:57:54 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace config {

class Redirect {
   public:
    std::uint32_t status_code;
    std::string   direction;
    std::string   origin;
};

class CgiPass {
   public:
    std::string path;
    std::string type;
};

class Location {
   public:
    void print(std::string prefix) const;

    std::string              path;
    std::vector<std::string> v_accepted_method;
    std::vector<Redirect>    v_redirect;
    std::string              root;
    bool                     directory_listing;
    std::vector<std::string> v_index;
    std::vector<Location>    v_location;
    std::vector<CgiPass>     v_cgi_pass;
};

}  // namespace config