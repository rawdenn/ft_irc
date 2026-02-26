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
        - INVITE <channel> <user>
        - PRIVMSG <channel> :<message>

*/

Commands::Commands()
{
    cmdMap["PASS"] = &Commands::handlePass;
    cmdMap["NICK"] = &Commands::handleNick;
    cmdMap["USER"] = &Commands::handleUser;
    cmdMap["QUIT"] = &Commands::handleQuit;
    cmdMap["JOIN"] = &Commands::handleJoin;
    cmdMap["PRIVMSG"] = &Commands::handlePrivmsg;
    cmdMap["TOPIC"] = &Commands::handleTopic;
    cmdMap["PART"] = &Commands::handlePart;
    cmdMap["KICK"] = &Commands::handleKick;
    cmdMap["INVITE"] = &Commands::handleInvite;
    cmdMap["MODE"] = &Commands::handleMode;
};

Commands::~Commands() {}

// for example, split "USER username 0 * :Real Name" into ["USER", "username", "0", "*", ":Real_Name"]
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

std::string toUpper(const std::string &input)
{
    std::string result = input;
    for (size_t i = 0; i < result.size(); i++)
        result[i] = std::toupper(result[i]);
    return result;
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

    std::map<std::string, CommandHandler>::iterator it = cmdMap.find(toUpper(command));
    if (it != cmdMap.end())
    {
        CommandHandler handler = it->second;
        (this->*handler)(server, client, tokens);
    }
    else
    {
        sendNumeric(client, "421", server.getName(), command + " :Unknown command");
    }
}
