/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tjensen <tjensen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 10:49:36 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/24 17:03:08 by tjensen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define SSTR_DEC(x) static_cast<std::ostringstream&>((std::ostringstream() << std::dec << x)).str()
#define SSTR_HEX(x) static_cast<std::ostringstream&>((std::ostringstream() << std::hex << x)).str()

namespace core {

std::string timestamp();

}
