#include "../includes/Commands.hpp"


static bool isValidChannelName(const std::string &name)
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

bool checkRegistered(Server &server, Client &client)
{
    if (!client.isRegistered())
    {
        sendNumeric(client, "451", server.getName(), ":You have not registered");
        return false;
    }
    return true;
}

bool validateParams(Server &server, Client &client, const std::vector<std::string> &params, size_t params_num)
{
    if (params.size() < params_num)
    {
        sendNumeric(client, "461", server.getName(), "JOIN :Not enough parameters");
        return false;
    }
    return true;
}

static bool ValidateChannelName(std::string chanName, Server &server, Client &client)
{
    if (!isValidChannelName(chanName))
    {
        sendNumeric(client, "476", server.getName(), client.getNickname() + " " + chanName + " :Bad Channel Mask");
        return false;
    }
    return true ;

}

static bool validateKey(Server &server, Client &client, const std::vector<std::string> &params, Channel *channel)
{
    if (channel->getHasKey())
    {
        if (params.size() <= 2)
        {
            //change error message
            sendNumeric(client, "numeric", server.getName(), client.getNickname() + " " + channel->getName() + " :MISSING PASS!!");
            return false;
        }
        if (params.size() > 2 && channel->getKey() != params[2])
        {
            //change error message
            sendNumeric(client, "numeric", server.getName(), client.getNickname() + " " + channel->getName() + " :WRONG PASS!!");
            return false;
        }
    }
    return true;
}

static bool canJoinChannel(Server &server, Client &client, Channel *channel)
{
    if (channel->hasMember(client.getFd()))
    {
        sendNumeric(client, "443", server.getName(), client.getNickname() + " " + channel->getName() + " :is already on channel");
        return false;
    }
    if (channel->getIsInviteOnly() && !channel->isinvited(client.getFd()))
    {
        //change error message to correct format
        sendNumeric(client, "numeric", server.getName(), client.getNickname() + " " + channel->getName() + " :cannot join channel, not invited and channel is invite only");
        return false;
    }
    if (channel->getUserNumber() == channel->getUserLimit())
    {
        std::cout << "user number " << channel->getUserNumber() << "user limit: " << channel->getUserLimit() << std::endl;
        //change error message to correct format
        sendNumeric(client, "numeric", server.getName(), client.getNickname() + " " + channel->getName() + " :cannot join channel, channel is full");
        return false;
    }
    return (true);
}

void Commands::handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!checkRegistered(server, client)
        || !validateParams(server, client, params, 2))
        return;

    std::string chanName = params[1];
    if (!ValidateChannelName(chanName, server, client))
        return ;

    Channel *channel = server.findChannel(chanName);

    if (!channel)
        channel = server.createChannel(chanName, client);
    else
    {
        if (!validateKey(server, client, params, channel)
            || !canJoinChannel(server, client, channel))
            return ;
        channel->addMember(&client);
        channel->incrementUserNumber();
        channel->removeFromInvitedMembersList(client.getFd());
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
