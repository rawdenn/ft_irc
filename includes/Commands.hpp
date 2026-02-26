#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Server.hpp"
#include "Client.hpp"

class Commands
{
private:
    typedef void (Commands::*CommandHandler)(Server &, Client &, const std::vector<std::string> &);
    std::map<std::string, CommandHandler> cmdMap;

public:
    Commands();
    ~Commands();
    void execute(Server &server, Client &client, std::string &cmd);

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
    void handleMode(Server &server, Client &client, const std::vector<std::string> &params);
    void parseModes(Server &server, Client &client, Channel *channel, const std::vector<std::string> &params);
};
void    sendNumeric(Client &client, const std::string &numeric, const std::string &server_name, const std::string &message);
bool    validateParams(Server &server, Client &client, const std::vector<std::string> &params, size_t params_num);
bool    checkRegistered(Server &server, Client &client);
#endif