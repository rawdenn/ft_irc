#include "../includes/Client.hpp"

Client::Client(int fd)
{
    this->fd = fd;
    this->nickname = "";
    this->username = "";
    this->realname = "";
    this->buffer = "";
    this->registered = false;
    this->passAccepted = false;
}

Client::Client(int fd, const std::string& nick, const std::string& user, const std::string& real)
{
    this->fd = fd;
    this->nickname = nick;
    this->username = user;
    this->realname = real;
    this->buffer = "";
    this->registered = false;
    this->passAccepted = false;
}

Client::~Client()
{
}

int Client::getFd() const
{
    return this->fd;
}

bool Client::isRegistered() const
{
    return this->registered;
}

bool Client::isPassAccepted() const
{
    return this->passAccepted;
}

const std::string& Client::getNickname() const
{
    return this->nickname;
}

const std::string& Client::getUsername() const
{
    return this->username;
}

const std::string& Client::getRealname() const
{
    return this->realname;
}

void Client::setNickname(const std::string& nick)
{
    this->nickname = nick;
}

void Client::setUsername(const std::string& user)
{
    this->username = user;
}

void Client::setRealname(const std::string& real)
{
    this->realname = real;
}

void Client::setPassAccepted(bool val)
{
    this->passAccepted = val;
}

void Client::setRegistered(bool val)
{
    this->registered = val;
}

void Client::appendBuffer(const std::string& data)
{
    this->buffer += data;
}

// Check if the buffer contains a complete command
bool Client::hasCompleteCommand() const
{
    return this->buffer.find("\r\n") != std::string::npos;
}

// Extract the command from the buffer
std::string Client::extractCommand()
{
    std::string command;
    std::size_t pos = this->buffer.find("\r\n");
    if (pos != std::string::npos)
    {
        command = this->buffer.substr(0, pos);
        this->buffer.erase(0, pos + 2); // remove command with \r\n
    }
    return command;
}

/*
    Commands before registration:
        - PASS <password>
        - NICK <nickname>
        - USER <username> 0 * :<realname>

    After registration:
        - JOIN <channel>
        - PART <channel>
        - TOPIC <channel> :<topic>
        - KICK <channel> <user>
        - MODE <channel> <mode> <user>

*/