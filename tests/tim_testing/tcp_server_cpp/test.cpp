#include <iostream>
#include <string>

#include "Request.hpp"

int main(int argc, char **argv) {
    Request request;

    int status = request.parse_request_line(argv[1], strlen(argv[1]));
    if (status)
        std::cerr << "invalid [" << status << "]\n";
    else
        std::cerr << "valid [" << status << "]\n";
    return status;
}
