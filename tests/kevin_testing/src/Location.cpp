/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 16:01:57 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/09 16:34:22 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

namespace ft {

void Location::print(std::string prefix) const {
    for (std::vector<LocationPath>::const_iterator it = v_path.begin(); it != v_path.end(); ++it) {
        std::cout << prefix << "path: " << it->str << " is a " << wildcard_string[it->wildcard]
                  << " wildcard\n";
    }
    for (std::vector<std::string>::const_iterator it = v_accepted_method.begin();
         it != v_accepted_method.end(); ++it) {
        std::cout << prefix << "accepted_methods: " << *it << "\n";
    }
    for (std::vector<std::string>::const_iterator it = v_redirect.begin(); it != v_redirect.end();
         ++it) {
        std::cout << prefix << "redirect: " << *it << "\n";
    }
    if (root.size() > 0)
        std::cout << prefix << "root: " << root << "\n";
    std::cout << prefix << "directory_listing: " << directory_listing << "\n";
    for (std::vector<std::string>::const_iterator it = v_index.begin(); it != v_index.end(); ++it) {
        std::cout << prefix << "index: " << *it << "\n";
    }
    for (std::vector<std::string>::const_iterator it = v_cgi_pass.begin(); it != v_cgi_pass.end();
         ++it) {
        std::cout << prefix << "cgi_pass: " << *it << "\n";
    }
    for (std::vector<Location>::const_iterator it = v_location.begin(); it != v_location.end();
         ++it) {
        std::cout << prefix << "Location:\n";
        std::cout << prefix << "{\n";
        it->print(prefix + "    ");
        std::cout << prefix << "}\n";
    }
}
}  // namespace ft