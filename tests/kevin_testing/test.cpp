/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/12 14:40:05 by khirsig           #+#    #+#             */
/*   Updated: 2022/09/13 08:43:14 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    // std::ifstream     i("/dev/urandom");
    // std::stringstream buf;
    // int               ii;

    // i >> ii;

    // std::cout << ii << "\n";

    std::vector<int> v;

    v.assign(5, 10);

    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << "  " << v.size() << std::endl;
    }

    std::vector<int> vi;

    std::vector<int>::iterator iter = v.begin();
    ++iter;
    ++iter;
    vi.assign(iter, v.end() - 1);

    for (std::vector<int>::iterator it = vi.begin(); it != vi.end(); ++it) {
        std::cout << *it << "  " << vi.size() << std::endl;
    }
    vi.clear();
    std::cout << "\n" << vi.size() << "\n";

    // std::vector<int>::iterator it = v.begin();
    // while (it == v.end() - 4) {
    //     std::cout << *it << std::endl;
    //     ++it;
    // }
    return 0;
}

// 5(begin), 4, 3, 2, (end)