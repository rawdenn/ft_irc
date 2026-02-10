#include "Server.hpp"

Server::Server(int port, const std::string &password)
{
    // check if valid later
    this->password = password;
    this->running = false;

    //we create socket here...
}