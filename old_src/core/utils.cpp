/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 10:50:44 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/10 11:03:54 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

namespace core {

std::string timestamp() {
    std::time_t t = time(NULL);
    struct tm  *time_master = localtime(&t);
    std::string ret;

    std::cerr << time_master->tm_year + 1900 << "/";
    if (time_master->tm_mon + 1 < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_mon + 1 << "/";
    if (time_master->tm_mday < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_mday << " ";
    if (time_master->tm_hour < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_hour << ":";
    if (time_master->tm_min < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_min << ":";
    if (time_master->tm_sec < 10)
        std::cerr << "0";
    std::cerr << time_master->tm_sec << " ";
    return ret;
}

}  // namespace core
