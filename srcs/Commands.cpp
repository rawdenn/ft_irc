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
    cmdMap["KICK"] = &Commands::handlekick;
    // not implemented yet
};

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

    if (params.size() < 5)
    {
        sendNumeric(client, "461", server.getName(), "USER :Not enough parameters");
        return;
    }

    if (params[4][0] != ':')
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
            it->second.broadcast(client.getFd(), client.getNickname() + " has quit the server\n"); // ADD WHAT CHANNEL THIS IS IN
            it->second.removeMember(client.getFd());
        }
    }
    server.removeClient(client.getFd());
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
    if (chanName.empty() || chanName[0] != '#')
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
    }

    // not sure if it should always be @localhost
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN " + chanName + "\r\n";

    channel->broadcast(client.getFd(), joinMsg);

    if (!channel->getTopic().empty())
    {
        sendNumeric(client, "332", server.getName(), client.getNickname() + " " + chanName + " :" + channel->getTopic());
    }
    else
    {
        sendNumeric(client, "331", server.getName(), client.getNickname() + " " + chanName + " :No topic is set");
    }

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

void Commands::handlePrivmsg(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 3)
    {
        sendNumeric(client, "461", server.getName(), "PRIVMSG :Not enough parameters");
        return;
    }

    if (params[1].c_str()[0] != '#')
    {
        // this is wrong, need to check again
        sendNumeric(client, "479", server.getName(), params[1] + " :Bad channel name");
        return;
    }

    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[1] + " :No such channel");
        return;
    }
    std::string fullMsg = ":" + client.getNickname() + "!" + server.getName() + " PRIVMSG" + " " + channel->getName() + " :" + concatinate_params(params, 2) + "\r\n";
    channel->broadcast(client.getFd(), fullMsg);
}

void Commands::handlePart(Server &server, Client &client, const std::vector<std::string> &params)
{
    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        std::string message = ":" + client.getNickname() + "!" + server.getName() + " :" + params[1] + " channel does not exist\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
    }
    else
    {
        channel->removeOperator(client.getFd());
        std::string fullMsg = ":" + client.getNickname() + " left " + channel->getName() + "\r\n";
        channel->broadcast(client.getFd(), fullMsg);
        channel->removeMember(client.getFd());
    }
}

void Commands::handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "461", server.getName(), "TOPIC :Not enough parameters");
        return;
    }

    if (params[1].c_str()[0] != '#')
    {
        // this is wrong, need to check again
        sendNumeric(client, "479", server.getName(), params[1] + " :Bad channel name");
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

    if (params.size() == 2)
    {
        std::string message = ":" + client.getNickname() + "!" + server.getName() + " :" + channel->getTopic() + "\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
    }
    else
    {
        // check if input starts with a :
        // check if topic can be more than 1 word. if yes concatinate the sentence in 1 and set it
        if (channel->getIsTopicRestricted() && !channel->isOperator(client.getFd()))
        {
            sendNumeric(client, "482", server.getName(), channel->getName() + " :You're not an operator");
        }
        if (params[2].size() > 0)
            channel->setTopic(params[2]);
    }
}

// KICK #channel nickname :optional reason
void Commands::handlekick(Server &server, Client &client, const std::vector<std::string> &params)
{
    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        sendNumeric(client, "403", server.getName(), params[1] + " :No such channel");
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
        channel->broadcast(client.getFd(), fullMsg);
        // channel->removeMember(KickedClient.getFd());
    }
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

    std::map<std::string, CommandHandler>::iterator it = cmdMap.find(command);
    if (it != cmdMap.end())
    {
        CommandHandler handler = it->second;
        (this->*handler)(server, client, tokens);
    }
    else
    {
        // error message unknown command
    }
}
