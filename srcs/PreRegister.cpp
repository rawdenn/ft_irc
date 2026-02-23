#include "../includes/Commands.hpp"

void Commands::sendNumeric(Client &client, const std::string &numeric, const std::string &server_name, const std::string &message)
{
    std::string fullMsg = ":" + server_name + " " + numeric + " " + client.getNickname() + " " + message + "\r\n";
    send(client.getFd(), fullMsg.c_str(), fullMsg.size(), 0);
}

void Commands::sendWelcome(Server &server, Client &client)
{
    //add info about server in 004
    sendNumeric(client, "001", server.getName(), "Welcome to the IRC Network " + client.getNickname());
    sendNumeric(client, "002", server.getName(), "Your host is " + server.getName());
    sendNumeric(client, "003", server.getName(), "This server was created " + server.getCreationDate());
    sendNumeric(client, "004", server.getName(), "... server info ...");
}

void Commands::handlePass(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (client.hasSentPass)
    {
        sendNumeric(client, "462", server.getName(), "You may not reregister");
        return;
    }

    if (params.size() < 2)
    {
        sendNumeric(client, "461", server.getName(), "PASS :Not enough parameters");
        return;
    }

    // if there's more it should only check for 1st param
    if (params[1] != server.getPassword())
    {
        sendNumeric(client, "464", server.getName(), "Password incorrect");
        return;
    }

    client.hasSentPass = true;
    if (client.isRegistered() && !client.isWelcomeSent())
    {
        sendWelcome(server, client);
        client.setWelcomeSent(true);
    }
}

// These should not pass
// NICK 123rawan
// NICK @rawan
// NICK rawan!
// NICK rawan rawan
bool isValidNick(const std::string &nick)
{
    if (nick.empty())
        return false;
    if (!isalpha(nick[0]))
        return false;
    for (size_t i = 0; i < nick.size(); i++)
    {
        if (!isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-')
            return false;
    }
    return true;
}

void Commands::handleNick(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "431", server.getName(), "No nickname given");
        return;
    }

    std::string nickname = params[1];

    if (!isValidNick(nickname))
    {
        sendNumeric(client, "432", server.getName(), nickname + " :Erroneous nickname");
        return;
    }

    if (server.isNicknameTaken(nickname))
    {
        sendNumeric(client, "433", server.getName(), nickname + " :Nickname is already in use");
        return;
    }

    client.setNickname(nickname);
    client.hasSentNick = true;

    if (client.isRegistered() && !client.isWelcomeSent())
    {
        sendWelcome(server, client);
        client.setWelcomeSent(true);
    }
}

void Commands::handleUser(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (client.hasSentUser)
    {
        sendNumeric(client, "462", server.getName(), "You may not reregister");
        return;
    }

    if (params.size() < 5 || params[4][0] != ':')
    {
        sendNumeric(client, "461", server.getName(), "USER :Not enough parameters");
        return;
    }

    std::string realname;
    realname = params[4].substr(1);
    for (size_t i = 5; i < params.size(); ++i)
        realname += " " + params[i];

    if (realname.empty())
    {
        sendNumeric(client, "461", server.getName(), "USER :Not enough parameters2");
        return;
    }

    client.setUsername(params[1]);
    client.setRealname(realname);
    client.hasSentUser = true;

    if (client.isRegistered() && !client.isWelcomeSent())
    {
        sendWelcome(server, client);
        client.setWelcomeSent(true);
    }
}
