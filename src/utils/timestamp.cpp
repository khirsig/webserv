#include "timestamp.hpp"

#include <iostream>

namespace utils {

void print_timestamp(std::ostream& os) {
    std::time_t t = time(NULL);
    struct tm*  time_master = localtime(&t);

    os << time_master->tm_year + 1900 << "/";
    if (time_master->tm_mon + 1 < 10)
        os << "0";
    os << time_master->tm_mon + 1 << "/";
    if (time_master->tm_mday < 10)
        os << "0";
    os << time_master->tm_mday << " ";
    if (time_master->tm_hour < 10)
        os << "0";
    os << time_master->tm_hour << ":";
    if (time_master->tm_min < 10)
        os << "0";
    os << time_master->tm_min << ":";
    if (time_master->tm_sec < 10)
        os << "0";
    os << time_master->tm_sec;
}

}  // namespace utils
