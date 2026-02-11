// channel name
// list of clients
// operators
// topic

// Channel handles:
// JOIN
// PART
// TOPIC
// KICK
// MODE (later)

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class Client;

class Channel
{
private:
    std::string name;
    std::string topic;
    // std::map<int, Client *> clients;
    std::vector<int> operators;
    std::string key;
    int userLimit;

public:
    Channel();
    ~Channel();
    
    void addClient(Client *client);
    void removeClient(int fd);
    void broadcast(std::string message, int senderFd);
    bool isOperator(int fd);
};


#endif