#include "Server.hpp"
#include "Client.hpp"

class Commands
{
public:
    Commands();
    ~Commands();
    void execute(Server& server, Client& client, std::string& cmd);

    void sendWelcome(Server &server, Client &client);
    void handlePass(Server &server, Client &client, const std::string &param);
    void handleNick(Server &server, Client &client, const std::vector<std::string> &param);
    void handleUser(Server &server, Client &client, const std::vector<std::string> &param);
};

