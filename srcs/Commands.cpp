#include "../includes/Commands.hpp"

/*
    Commands before registration:
        - PASS <password>
        - NICK <nickname>
        - USER <username> 0 * :<realname>

    After registration:
        - JOIN <channel>
        - PART <channel> //not sure if it should be implemented yet
        - TOPIC <channel> :<topic>
        - KICK <channel> <user>
        - MODE <channel> <mode> <user>
        - INVITE <channel> <user>
        - PRIVMSG <channel> :<message>

*/

// need to check when a client disconnect, if they joined channels, we need to remove them from the channels

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
    // not implemented yet
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

void Commands::handleQuit(Server &server, Client &client, const std::vector<std::string> &params)
{
    (void)params;
    std::map<std::string, Channel>::iterator it;
    for (it = server.getChannels().begin(); it != server.getChannels().end(); it++)
    {
        if (it->second.hasMember(client.getFd()))
        {
            if (it->second.isOperator(client.getFd()))
                it->second.removeOperator(client.getFd());
            it->second.broadcastExcept(client.getFd(), client.getNickname() + " has quit the server\n"); // ADD WHAT CHANNEL THIS IS IN
            it->second.removeMember(client.getFd());
        }
    }
    server.removeClient(client.getFd());
}

bool isValidChannelName(const std::string &name)
{
    if (name.empty() || name[0] != '#')
        return false;
    if (!isalpha(name[1]))
        return false;
    for (size_t i = 1; i < name.size(); i++)
    {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-')
            return false;
    }
    return true;
}

// fixed some things but not sure if it covers all cases
void Commands::handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, "451", server.getName(), ":You have not registered");
        return;
    }

    if (params.size() < 2)
    {
        sendNumeric(client, "461", server.getName(), "JOIN :Not enough parameters");
        return;
    }

    std::string chanName = params[1];
    if (!isValidChannelName(chanName))
    {
        sendNumeric(client, "476", server.getName(), client.getNickname() + " " + chanName + " :Bad Channel Mask");
        return;
    }

    Channel *channel = server.findChannel(chanName);

    if (!channel)
        channel = server.createChannel(chanName, client);
    else
    {
        if (channel->hasMember(client.getFd()))
        {
            sendNumeric(client, "443", server.getName(), client.getNickname() + " " + chanName + " :is already on channel");
            return;
        }
        channel->addMember(&client);

        // just testing operators
        // if (channel->getOperators().empty())
        //     channel->addOperator(client.getFd());
    }

    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN " + chanName + "\r\n";

    channel->broadcast(joinMsg);

    if (!channel->getTopic().empty())
        sendNumeric(client, "332", server.getName(), client.getNickname() + " " + chanName + " :" + channel->getTopic());
    else
        sendNumeric(client, "331", server.getName(), client.getNickname() + " " + chanName + " :No topic is set");

    std::string names = channel->getNamesList();
    sendNumeric(client, "353", server.getName(), client.getNickname() + " = " + chanName + " :" + names);
    sendNumeric(client, "366", server.getName(), client.getNickname() + " " + chanName + " :End of /NAMES list");
}

static std::string concatinate_params(const std::vector<std::string> &params, int start)
{
    std::string message = "";
    for (size_t i = start; i < params.size(); i++)
    {
        message += params[i];
        if (i + 1 != params.size())
            message += " ";
    }
    return (message);
}

void Commands::handlePrivmsg(Server &server,
                             Client &client,
                             const std::vector<std::string> &params)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, "451", server.getName(), ":You have not registered");
        return;
    }

    if (params.size() < 3)
    {
        sendNumeric(client, "461", server.getName(), "PRIVMSG :Not enough parameters");
        return;
    }

    std::string message = concatinate_params(params, 2);
    if (message.empty())
    {
        sendNumeric(client, "412", server.getName(), ":No text to send");
        return;
    }

    std::string target = params[1];
    if (target[0] == '#')
    {
        Channel *channel = server.findChannel(target);
        if (!channel)
        {
            sendNumeric(client, "403", server.getName(), target + " :No such channel");
            return;
        }
        if (!channel->hasMember(client.getFd()))
        {
            sendNumeric(client, "442", server.getName(), target + " :You're not on that channel");
            return;
        }
        std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        channel->broadcastExcept(client.getFd(), fullMsg);
    }
    else
    {
        Client *recipient = server.getClientFromNickname(target);
        if (!recipient)
        {
            sendNumeric(client, "401", server.getName(), target + " :No such nick");
            return;
        }
        std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " + recipient->getNickname() + " :" + message + "\r\n";
        send(recipient->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
    }
}

void Commands::handlePart(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "461", server.getName(), "PART :Not enough parameters");
        return;
    }

    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[1] + " :No such channel");
        return;
    }

    channel->removeOperator(client.getFd());
    std::string fullMsg = ":" + client.getNickname() + " left " + channel->getName() + "\r\n";
    channel->broadcastExcept(client.getFd(), fullMsg);
    channel->removeMember(client.getFd());
}

void Commands::handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 3)
    {
        sendNumeric(client, "461", server.getName(), "TOPIC :Not enough parameters");
        return;
    }

    if (params[1].c_str()[0] != '#')
    {
        sendNumeric(client, "476", server.getName(), params[1] + " :Bad Channel Mask");
        return;
    }
    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[1] + " :No such channel");
        return;
    }

    if (!channel->hasMember(client.getFd()))
    {
        sendNumeric(client, "442", server.getName(), params[1] + " :You're not on that channel");
        return;
    }

    // check if input starts with a :
    // check if topic can be more than 1 word. if yes concatinate the sentence in 1 and set it
    if (channel->getIsTopicRestricted() && !channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "482", server.getName(), channel->getName() + " :You're not an operator");
    }
    if (params[2].size() > 0)
        channel->setTopic(params[2]);
}

// KICK #channel nickname :optional reason
void Commands::handleKick(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 3)
    {
        sendNumeric(client, "461", server.getName(), "KICK :Not enough parameters");
        return;
    }
    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[1] + " :No such channel");
        return;
    }
    Client *possibleMember = channel->getMemberFromNickname(params[2]);
    if (possibleMember == NULL)
    {
        sendNumeric(client, "441", server.getName(), params[2] + " " + params[1] + " :They aren't on that channel");
        return;
    }
    // check if param[2] is a member of server then check if memeber of channel
    if (channel->isOperator(client.getFd()))
    {
        std::string kickMessage = "";
        if (params.size() > 3)
        {
            kickMessage += concatinate_params(params, 3);
        }
        kickMessage += "\r\n";
        std::string fullMsg = ":" + client.getNickname() + " kicked " + params[2] + " from " + channel->getName() + kickMessage;
        channel->broadcastExcept(client.getFd(), fullMsg);
        channel->removeMember(possibleMember->getFd());
    }
    else
    {
        sendNumeric(client, "482", server.getName(), params[1] + " :You're not an operator");
        return;
    }
}

// example: INVITE nickname #channel
void Commands::handleInvite(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 3)
    {
        sendNumeric(client, "461", server.getName(), params[1] + " :INVITE:Not enough parameters");
        return;
    }
    Channel *channel = server.findChannel(params[2]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[2] + " :No such channel");
        return;
    }
    Client *possibleMember = server.getClientFromNickname(params[1]);
    if (possibleMember == NULL)
    {
        sendNumeric(client, "401", server.getName(), params[1] + " :No such nick");
        return;
    }

    if (channel->getIsInviteOnly() && !channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "473", server.getName(), params[1] + " :Cannot join channel (+i)");
        return;
    }
    // should we broadcast a message here?
    channel->addMember(possibleMember);
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
