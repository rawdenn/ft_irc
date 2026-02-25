#include "../includes/Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->topic = "";
    this->key = "";
    this->userLimit = -1;
    this->userNumber = 0;
    this->isInviteOnly = false;
    this->isTopicRestricted = true;
    this->hasKey = false;
}

Channel::Channel(std::string _name)
{
    this->name = _name;
    this->topic = "";
    this->key = "";
    this->userLimit = -1;
    this->userNumber = 0;
    this->isInviteOnly = false;
    this->isTopicRestricted = true;
    this->hasKey = false;
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

const int &Channel::getUserNumber() const
{
    return (this->userNumber);
}

void Channel::incrementUserNumber()
{
    this->userNumber++;
}

void Channel::decrementUserNumber()
{
    this->userNumber--;
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


const bool &Channel::getHasKey() const
{
    return (this->hasKey);
}

void Channel::setHasKey(bool val)
{
    this->hasKey = val;
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

std::map<int, Client *> &Channel::getMembers()
{
    return members;
}

void Channel::removeMember(Client *client)
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
        // delete channel
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
std::set<int> &Channel::getOperators()
{
    return operators;
}

bool Channel::isinvited(int fd) const
{
    return (this->invitedMembers.find(fd) != this->invitedMembers.end());
}

void Channel::addToInvitedMembersList(int fd)
{
    invitedMembers.insert(fd);
}

void Channel::removeFromInvitedMembersList(int fd)
{
    invitedMembers.erase(fd);
}

void Channel::broadcast(const std::string &message)
{
    for (std::map<int, Client *>::iterator it = members.begin();
         it != members.end(); ++it)
    {
        send(it->second->getFd(), message.c_str(), message.size(), 0);
    }
}

void Channel::broadcastExcept(int senderFd, const std::string &message)
{
    for (std::map<int, Client *>::iterator it = members.begin();
         it != members.end(); ++it)
    {
        if (it->first != senderFd)
            send(it->second->getFd(), message.c_str(), message.size(), 0);
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