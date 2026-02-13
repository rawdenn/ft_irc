#include "../includes/Commands.hpp"

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

Commands::Commands() {};

Commands::~Commands() {}

// for example, split "USER username 0 * :Real Name" into ["USER", "username", "0", "*", ":Real Name"]
std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos)
    {
        if (end != start) // ignore empty parts
            tokens.push_back(s.substr(start, end - start));

        start = end + 1;
        end = s.find(delimiter, start);
    }

    if (start < s.length())
        tokens.push_back(s.substr(start));

    return tokens;
}

void sendNumeric(Client &client, const std::string &numeric, const std::string &server_name, const std::string &message)
{
    std::string fullMsg = ":" + server_name + " " + numeric + " " + client.getNickname() + " :" + message + "\r\n";
    send(client.getFd(), fullMsg.c_str(), fullMsg.size(), 0);
}


void Commands::sendWelcome(Server &server, Client &client)
{
    sendNumeric(client, "001", server.getName(), "Welcome to the IRC Network " + client.getNickname());
    sendNumeric(client, "002", server.getName(), "Your host is " + server.getName());
    sendNumeric(client, "003", server.getName(), "This server was created ...");
    sendNumeric(client, "004", server.getName(), "... server info ...");
}


void Commands::handlePass(Server &server, Client &client, const std::string &param)
{
    if (client.hasSentPass)
        return;

    // if wrong password, disconnect client
    if (param != server.getPassword())
    {
        sendNumeric(client, "464", server.getName(), ":Password incorrect");
        server.removeClient(client.getFd());
        return;
    }

    client.hasSentPass = true;
}

void Commands::handleNick(Server &server, Client &client, const std::string &nickname)
{
    if (nickname.empty())
    {
        sendNumeric(client, "431", server.getName(), ":No nickname given");
        return;
    }

    if (server.isNicknameTaken(nickname))
    {
        sendNumeric(client, "433", server.getName(), nickname + " :Nickname is already in use");
        return;
    }

    client.setNickname(nickname);
    client.hasSentNick = true;

    if (client.isRegistered())
    {
        sendWelcome(server, client);
    }
}

void Commands::handleUser(Server &server, Client &client, const std::vector<std::string>& params)
{
    if (client.hasSentUser)
    {
        sendNumeric(client, "462", server.getName(), "You may not reregister");
        return;
    }

    if (params.size() < 4)
    {
        sendNumeric(client, "461", server.getName(), "USER :Not enough parameters");
        return;
    }

    client.setUsername(params[1]);
    std::string realname;
    for (size_t i = 0; i < params.size(); ++i)
    {
        if (params[i][0] == ':')
        {
            realname = params[i].substr(1); // remove ':'
            for (size_t j = i + 1; j < params.size(); ++j)
                realname += " " + params[j];
            break;
        }
    }

    // std::cout << "Realname extracted: " << realname << std::endl;
    client.setRealname(realname);
    client.hasSentUser = true;

    if (client.isRegistered())
        sendWelcome(server, client);
}

void Commands::execute(Server &server, Client &client, std::string &cmd)
{
    std::vector<std::string> tokens = split(cmd, ' ');
    if (tokens.empty())
        return;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Token " << i << ": " << tokens[i] << std::endl;
    }
    
    std::string command = tokens[0];

    // TO-DO: replace this logic here later(after exec of all commands + adding them to a map)
    if (command == "PASS")
        handlePass(server, client, tokens[1]);
    else if (command == "NICK" && tokens.size() > 1)
        handleNick(server, client, tokens[1]);
    else if (command == "USER")
        handleUser(server, client, tokens);
    else if (!client.isPassAccepted())
    {
        // later
    }

    if (client.isRegistered()) // just testing
        std::cout << "Client is registered." << std::endl;

}
