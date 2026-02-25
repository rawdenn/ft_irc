#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "ft_irc.h"

class Channel
{
private:
    std::string name;
    std::string topic;
    std::map<int, Client *> members;
    std::set<int> operators;
    std::set<int> invitedMembers;
    std::string key;
    int userLimit;
    int userNumber;
    bool isInviteOnly;
    bool isTopicRestricted;
    bool hasKey;

public:
    Channel();
    Channel(std::string _name);
    ~Channel();

    const std::string &getName() const;
    Client *getMemberFromNickname(std::string nickname);

    const std::string &getTopic() const;
    void setTopic(std::string newTopic);

    const std::string &getKey() const;
    void setKey(std::string newKey);

    const int &getUserLimit() const;
    void setUserLimit(int newUserLimit);

    const int &getUserNumber() const;
    void incrementUserNumber();
    void decrementUserNumber();

    const bool &getIsInviteOnly() const;
    void setIsInviteOnly(bool val);

    const bool &getIsTopicRestricted() const;
    void setIsTopicRestricted(bool val);

    const bool &getHasKey() const;
    void setHasKey(bool val);

    std::map<int, Client *> &getMembers();
    std::set<int> &getOperators();

    bool isinvited(int fd) const;

    bool hasMember(int fd);
    bool isOperator(int fd) const;

    void addMember(Client *client);
    void removeMember(Client *client);
    void removeMember(int fd);

    void addOperator(int fd);
    void removeOperator(int fd);

    void addToInvitedMembersList(int fd);
    void removeFromInvitedMembersList(int fd);

    void broadcast(const std::string &message); // to everyone
    void broadcastExcept(int senderFd, const std::string &message);
    std::string getNamesList() const;

    bool isEmpty() const;
};

#endif