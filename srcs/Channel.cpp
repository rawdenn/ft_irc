#include "../includes/Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->topic = "";
    this->key = "";
    this->userLimit = 0;
}
Channel::Channel(std::string _name)
{
    this->name = _name;
    this->topic = "";
    this->key = "";
    this->userLimit = 0;
}
Channel::~Channel()
{}

const std::string &Channel::getName() const
{
    return this->name;
}

bool Channel::hasMember(int fd)
{
    return (this->members.find(fd) != this->members.end());
}

bool Channel::isOperator(int fd)
{
    return (this->operators.find(fd) != this->operators.end());
}

void Channel::addMember(Client *client)
{
    members.insert(std::make_pair(client->getFd(), client));
}
void Channel::removeMember(int fd)
{
    members.erase(fd);
    operators.erase(fd);
}

void Channel::addOperator(int fd)
{
    operators.insert(fd);
}
void Channel::removeOperator(int fd)
{
    operators.erase(fd);
}

void Channel::broadcast(int senderFd, const std::string &server_name, const std::string &message)
{
    std::map<int, Client *>::iterator it;

    for (it = this->members.begin(); it != this->members.end(); it++)
    {
        if (it->first == senderFd)
            continue;
        Client *client = it->second;
        std::string fullMsg = ":" + client->getNickname() + "!" + server_name
            + " PRIVMSG" + " #" + this->name + " :" + message + "\r\n";
            send(client->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
    }
}

bool Channel::isEmpty() const
{
    return (this->members.empty());
}