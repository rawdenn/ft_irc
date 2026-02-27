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
    this->welcomeSent = false;
    this->sentPass = false;
    this->sentNick = false;
    this->sentUser = false;
}

Client::~Client()
{}

int Client::getFd() const
{
    return this->fd;
}

bool Client::isRegistered() const
{
    return (hasSentNick() && hasSentUser() && hasSentPass()) || this->registered;
}

bool Client::isPassAccepted() const
{
    return this->passAccepted;
}

bool Client::isWelcomeSent() const
{
    return this->welcomeSent;
}

bool Client::hasSentPass() const
{
    return this->sentPass;
}

bool Client::hasSentNick() const
{
    return this->sentNick;
}

bool Client::hasSentUser() const
{
    return this->sentUser;
}

const std::string &Client::getNickname() const
{
    return this->nickname;
}

const std::string &Client::getUsername() const
{
    return this->username;
}

const std::string &Client::getRealname() const
{
    return this->realname;
}

void Client::setNickname(const std::string &nick)
{
    this->nickname = nick;
}

void Client::setUsername(const std::string &user)
{
    this->username = user;
}

void Client::setRealname(const std::string &real)
{
    this->realname = real;
}

void Client::setPassAccepted(bool val)
{
    this->passAccepted = val;
}
void Client::setWelcomeSent(bool val)
{
    this->welcomeSent = val;
}

void Client::setRegistered(bool val)
{
    this->registered = val;
}

void Client::setHasSentPass(bool val)
{
    this->sentPass = val;
}

void Client::setHasSentNick(bool val)
{
    this->sentNick = val;
}

void Client::setHasSentUser(bool val)
{
    this->sentUser = val;
}

void Client::appendBuffer(const std::string &data)
{
    this->buffer += data;
}

const std::string Client::getPrefix() const
{
    return nickname + "!" + username + "@localhost";
}

// run nc -C localhost 6667 la tzbat ma3 \r
bool Client::hasCompleteCommand() const
{
    return this->buffer.find("\r\n") != std::string::npos;
}

std::string Client::extractCommand()
{
    std::string command;
    std::size_t pos = this->buffer.find("\r\n");
    if (pos != std::string::npos)
    {
        command = this->buffer.substr(0, pos);
        // If the line ended with CRLF, remove the trailing '\r'
        // if (!command.empty() && command[command.size() - 1] == '\r')
        //     command.erase(command.size() - 1, 1);
        // // Erase the line including the '\n' (1 char). If it was CRLF, the '\r' was part of command and already removed.
        // this->buffer.erase(0, pos + 1);
        this->buffer.erase(0, pos + 2);
    }
    std::cout << "command is: " << command << std::endl;
    return command;
}
