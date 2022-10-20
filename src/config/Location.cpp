/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 16:01:57 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/20 17:03:45 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

namespace config {

void Location::print(std::string prefix) const {
    std::cout << prefix << "path: " << path << "\n";
    for (std::vector<std::string>::const_iterator it = v_accepted_method.begin();
         it != v_accepted_method.end(); ++it) {
        std::cout << prefix << "accepted_methods: " << *it << "\n";
    }
    for (std::vector<Redirect>::const_iterator it = v_redirect.begin(); it != v_redirect.end();
         ++it) {
        std::cout << prefix << "redirect\n" << prefix << "{\n";
        std::cout << prefix << "    status_code: \"" << it->status_code << "\"\n";
        std::cout << prefix << "    origin: \"" << it->origin << "\"\n";
        std::cout << prefix << "    direction: \"" << it->direction << "\"\n" << prefix << "}\n";
    }
    if (root.size() > 0)
        std::cout << prefix << "root: " << root << "\n";
    std::cout << prefix << "directory_listing: " << directory_listing << "\n";
    for (std::vector<std::string>::const_iterator it = v_index.begin(); it != v_index.end(); ++it) {
        std::cout << prefix << "index: " << *it << "\n";
    }
    for (std::vector<CgiPass>::const_iterator it = v_cgi_pass.begin(); it != v_cgi_pass.end();
         ++it) {
        std::cout << prefix << "cgi_pass: " << it->path << " " << it->type << "\n";
    }
    for (std::vector<Location>::const_iterator it = v_location.begin(); it != v_location.end();
         ++it) {
        std::cout << prefix << "Location:\n";
        std::cout << prefix << "{\n";
        it->print(prefix + "    ");
        std::cout << prefix << "}\n";
    }
}
}  // namespace config