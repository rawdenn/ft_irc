//this class is so that the code is cleaner
#include "Server.hpp"
#include "Client.hpp"

class Commands
{
private:
    Server &server;
public:
    Commands(Server &server);
    ~Commands();
    void execute(Server& server, Client& client, std::string& cmd);
};

