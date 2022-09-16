/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 14:10:30 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/16 09:05:04 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace config {

enum wildcard { NONE, PREFIX, POSTFIX };

static const char* wildcard_string[] = {"NONE", "PREFIX", "POSTFIX"};

class LocationPath {
   public:
    std::string str;
    wildcard    wildcard;
};

class Redirect {
   public:
    std::uint32_t status_code;
    std::string   direction;
    std::string   origin;
};

class Location {
   public:
    void print(std::string prefix) const;

    std::vector<LocationPath> v_path;
    std::vector<std::string>  v_accepted_method;
    std::vector<Redirect>     v_redirect;
    std::string               root;
    bool                      directory_listing;
    std::vector<std::string>  v_index;
    std::vector<Location>     v_location;
    std::vector<std::string>  v_cgi_pass;
};

}  // namespace config