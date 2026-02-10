// Creates socket
// bind()
// listen()
// accept()
// poll() loop
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
private:
    int serverFd;
    std::vector<pollfd> pollFds;
    std::map<int, Client> clients;  // key is fd
    std::map<std::string, Channel> channels;  // key is channel name
    std::string password;
    bool running; //for clean shutdown

public:
    Server(int port, const std::string &password);
    ~Server();

    void run();
    void shutdown();
    std::string getPassword() const;

    // Client management
    void acceptClient();
    void removeClient(int fd);

    // Command handling
    void handleClientMessage(int fd);
    void executeCommand(Client &client, std::string command);
    // void broadcastMessage(const std::string& message, int excludeFd = -1);

    // Channel management
    Channel* findChannel(const std::string &name);
};

#endif
