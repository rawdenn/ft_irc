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
# define CHANNEL_HPP

# include "Client.hpp"
# include "ft_irc.h"

class Channel
{
private:
    std::string name;
    std::string topic;
    std::map<int, Client *> members;
    std::set<int> operators;
    std::string key;
    int userLimit;
public:
    Channel();
    Channel(std::string _name);
    ~Channel();
    
    const std::string &getName() const;
    
    const std::string &getTopic() const;
    void setTopic(std::string newTopic);

    bool hasMember(int fd);
    bool isOperator(int fd);

    void addMember(Client *client);
    void removeMember(int fd);

    void addOperator(int fd);
    void removeOperator(int fd);
    
    void broadcast(int senderFd, const std::string &message);

    bool isEmpty() const;
};


#endif