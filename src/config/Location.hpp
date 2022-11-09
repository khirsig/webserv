/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 14:10:30 by khirsig           #+#    #+#             */
/*   Updated: 2022/11/09 15:34:44 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../settings.hpp"

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
    Location() : client_max_body_size(SIZE_MAX) {}
    void print(std::string prefix) const;

    std::string              path;
    std::vector<std::string> v_accepted_method;
    std::vector<Redirect>    v_redirect;
    std::string              root;

    std::uint64_t client_max_body_size;
    bool          directory_listing;

    std::vector<std::string> v_index;
    std::vector<Location>    v_location;
    std::vector<CgiPass>     v_cgi_pass;
};

}  // namespace config