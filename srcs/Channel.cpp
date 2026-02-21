#include "../includes/Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->topic = "";
    this->key = "";
    this->userLimit = -1;
    this->isInviteOnly =false;
    this->isTopicRestricted = true;
}
Channel::Channel(std::string _name)
{
    this->name = _name;
    this->topic = "";
    this->key = "";
    this->userLimit = -1;
    this->isInviteOnly =false;
    this->isTopicRestricted = true;
}
Channel::~Channel()
{
}

const std::string &Channel::getName() const
{
    return this->name;
}

const std::string &Channel::getTopic() const
{
    return this->topic;
}

void Channel::setTopic(std::string newTopic)
{
    this->topic = newTopic;
}

const std::string &Channel::getKey() const
{
    return (this->key);
}

void Channel::setKey(std::string newKey)
{
    this->key = newKey;
}

const int &Channel::getUserLimit() const
{
    return (this->userLimit);
}

void Channel::setUserLimit(int newUserLimit)
{
    this->userLimit = newUserLimit;
}

const bool &Channel::getIsInviteOnly() const
{
    return (this->isInviteOnly);
}

void Channel::setIsInviteOnly(bool val)
{
    this->isInviteOnly = val;
}

const bool &Channel::getIsTopicRestricted() const
{
    return (this->isTopicRestricted);
}

void Channel::setIsTopicRestricted(bool val)
{
    this->isTopicRestricted = val;
}

bool Channel::hasMember(int fd)
{
    return (this->members.find(fd) != this->members.end());
}

Client *Channel::getMemberFromNickname(std::string nickname)
{
    std::map<int, Client *>::iterator it;

    for (it = members.begin(); it != members.end(); ++it)
        if (it->second->getNickname() == nickname)
            return (it->second);
    return (NULL);
}


bool Channel::isOperator(int fd) const
{   
    return (this->operators.find(fd) != this->operators.end());
}

void Channel::addMember(Client *client)
{
    members.insert(std::make_pair(client->getFd(), client));
}

std::map<int, Client*>& Channel::getMembers()
{
    return members;
}

void Channel::removeMember(Client* client)
{
    members.erase(client->getFd());
    operators.erase(client->getFd());
}

void Channel::removeMember(int fd)
{
    operators.erase(fd);
    members.erase(fd);
    if (members.empty())
    {
        //delete channel
    }
}

void Channel::addOperator(int fd)
{
    operators.insert(fd);
}
void Channel::removeOperator(int fd)
{
    operators.erase(fd);
}

void Channel::broadcast(int senderFd, const std::string &message)
{
    std::map<int, Client *>::iterator it;

    for (it = this->members.begin(); it != this->members.end(); it++)
    {
        if (it->first == senderFd)
            continue;
        Client *client = it->second;
        send(client->getFd(), message.c_str(), message.size(), 0);
    }
}

std::string Channel::getNamesList() const
{
    std::string namesList = "";
    std::map<int, Client *>::const_iterator it;

    for (it = this->members.begin(); it != this->members.end(); it++)
    {
        Client *client = it->second;
        if (isOperator(client->getFd()))
            namesList += "@" + client->getNickname() + " ";
        else
            namesList += client->getNickname() + " ";
    }
    return namesList;
}

bool Channel::isEmpty() const
{
    return (this->members.empty());
}