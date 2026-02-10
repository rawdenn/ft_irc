#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <cstdio>
#include "Server.hpp"

#define BUFFER_SIZE 1024

//t5ati hol, this is a test from chat
//   |
//   |
//   |
//  \ /
//   -


// int setNonBlocking(int fd)
// {
//     int flags = fcntl(fd, F_GETFL, 0);
//     if (flags == -1)
//         return -1;
//     return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// }

// int main(int argc, char **argv)
// {
//     if (argc != 2)
//     {
//         std::cerr << "Usage: ./ircserv <port>" << std::endl;
//         return 1;
//     }

//     int port = atoi(argv[1]);

//     // Create socket
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0)
//     {
//         perror("socket");
//         return 1;
//     }

//     // Allow reuse of port
//     int opt = 1;
//     setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     // Setup address
//     sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(port);

//     // Bind
//     if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
//     {
//         perror("bind");
//         return 1;
//     }

//     // Listen
//     if (listen(server_fd, SOMAXCONN) < 0)
//     {
//         perror("listen");
//         return 1;
//     }

//     // Make server socket non-blocking
//     setNonBlocking(server_fd);

//     std::vector<pollfd> fds;

//     pollfd server_poll;
//     server_poll.fd = server_fd;
//     server_poll.events = POLLIN;
//     fds.push_back(server_poll);

//     std::cout << "Server started on port " << port << std::endl;

//     while (true)
//     {
//         if (poll(&fds[0], fds.size(), -1) < 0)
//         {
//             perror("poll");
//             break;
//         }

//         for (size_t i = 0; i < fds.size(); i++)
//         {
//             // New connection
//             if (fds[i].fd == server_fd && (fds[i].revents & POLLIN))
//             {
//                 sockaddr_in client_addr;
//                 socklen_t client_len = sizeof(client_addr);

//                 int client_fd = accept(server_fd,
//                     (sockaddr*)&client_addr,
//                     &client_len);

//                 if (client_fd >= 0)
//                 {
//                     setNonBlocking(client_fd);

//                     pollfd client_poll;
//                     client_poll.fd = client_fd;
//                     client_poll.events = POLLIN;
//                     fds.push_back(client_poll);

//                     std::cout << "New client connected: "
//                               << client_fd << std::endl;
//                 }
//             }
//             // Client sent data
//             else if (fds[i].revents & POLLIN)
//             {
//                 char buffer[BUFFER_SIZE];
//                 memset(buffer, 0, BUFFER_SIZE);

//                 int bytes = recv(fds[i].fd, buffer, BUFFER_SIZE - 1, 0);

//                 if (bytes <= 0)
//                 {
//                     std::cout << "Client disconnected: "
//                               << fds[i].fd << std::endl;
//                     close(fds[i].fd);
//                     fds.erase(fds.begin() + i);
//                     i--;
//                 }
//                 else
//                 {
//                     std::cout << "Message from "
//                               << fds[i].fd << ": "
//                               << buffer;
//                 }
//             }
//         }
//     }

//     close(server_fd);
//     return 0;
// }

int main(int argc, char const **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    try
    {
        int port = std::atoi(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
