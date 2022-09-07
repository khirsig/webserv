// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#define PORT 9001
#define BUFFER_SIZE 4096

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        std::cerr << "usage:\n"
                  << "./tcp_client 127.0.0.1 9001 MSG_DATA" << std::endl;
        return 1;
    }

    // Create socket
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address / address not supported" << std::endl;
        return 1;
    }

    //
    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return 1;
    }

    char *buffer = new char[BUFFER_SIZE + 1];
    send(socket_fd, argv[3], strlen(argv[3]), 0);
    ssize_t read_bytes = BUFFER_SIZE;
    while (read_bytes == BUFFER_SIZE)
    {
        read_bytes = read(socket_fd, buffer, BUFFER_SIZE);
        if (read_bytes < 0)
        {
            perror("read");
            delete[] buffer;
            return 1;
        }
        buffer[read_bytes] = '\0';
        std::cout << buffer;
    }
    // std::cout << std::endl;
    delete[] buffer;
    return 0;
}
