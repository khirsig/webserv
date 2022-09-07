// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "usage:\n./tcp_client 127.0.0.1 9001 MSG_DATA\n");
        return 1;
    }

    // Create socket
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0)
    {
        perror("Connection Failed");
        return 1;
    }

    // send request
    size_t send_bytes = strlen(argv[3]);
    if (send(socket_fd, argv[3], send_bytes, 0) != send_bytes)
    {
        perror("send");
        return 1;
    }

    // fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    char read_line[BUFFER_SIZE + 1];
    ssize_t read_bytes;
    while ((read_bytes = read(socket_fd, read_line, BUFFER_SIZE)) > 0)
    {
        read_line[read_bytes] = '\0';
        printf("%s", read_line);
    }

    struct pollfd fds[1];
    fds[0].fd = socket_fd;
    fds[0].events = POLLIN | POLLHUP;

    // while (1)
    // {
    //     if (poll(fds, 1, -1) > 0)
    //     {
    //         if (fds[0].revents & POLLIN)
    //         {
    //             char read_line[BUFFER_SIZE + 1];
    //             ssize_t read_bytes = recv(socket_fd, read_line, BUFFER_SIZE, 0);
    //             if (read_bytes <= 0)
    //                 break;
    //             read_line[read_bytes] = '\0';
    //             printf("%s", read_line);
    //         }
    //         else if (fds[0].revents & POLLHUP)
    //         {
    //             fprintf(stderr, "fds[0].revents & POLLHUP\n");
    //         }
    //     }
    //     else
    //     {
    //         perror("poll");
    //         exit(1);
    //     }
    // }

    close(socket_fd);
    return 0;
}
