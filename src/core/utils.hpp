/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 10:49:36 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/20 19:57:25 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define SSTR(x) static_cast<std::ostringstream&>((std::ostringstream() << std::dec << x)).str()

namespace core {

std::string timestamp();

}
