//this class is so that the code is cleaner
#include "Server.hpp"
#include "Client.hpp"

class Commands
{
public:
    Commands();
    ~Commands();
    void execute(Server& server, Client& client, std::string& cmd);
};

