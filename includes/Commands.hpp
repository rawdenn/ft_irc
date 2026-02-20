#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "Server.hpp"
# include "Client.hpp"

class Commands
{

public:
    typedef void (Commands::*CommandHandler)(Server&, Client&, const std::vector<std::string>&);
    Commands();
    ~Commands();
    void execute(Server& server, Client& client, std::string& cmd);

    void sendNumeric(Client &client, const std::string &numeric, const std::string &server_name, const std::string &message);
    void sendWelcome(Server &server, Client &client);
    void handlePass(Server &server, Client &client, const std::vector<std::string> &params);
    void handleNick(Server &server, Client &client, const std::vector<std::string> &params);
    void handleUser(Server &server, Client &client, const std::vector<std::string> &params);
    void handleQuit(Server &server, Client &client, const std::vector<std::string> &params);
    void handleJoin(Server &server, Client &client, const std::vector<std::string> &params);
    void handlePrivmsg(Server &server, Client &client, const std::vector<std::string> &params);
    void handleTopic(Server &server, Client &client, const std::vector<std::string> &params);
    void handlePart(Server &server, Client &client, const std::vector<std::string> &params);
    void handleKick(Server &server, Client &client, const std::vector<std::string> &params);
    void handleInvite(Server &server, Client &client, const std::vector<std::string> &params);
    private:
    std::map<std::string, CommandHandler> cmdMap;
};

#endif