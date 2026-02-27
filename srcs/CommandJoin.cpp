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

bool validateParams(Server &server, Client &client, const std::vector<std::string> &params, size_t params_num, const std::string &cmd)
{
    if (params.size() < params_num)
    {
        sendNumeric(client, "461", server.getName(), cmd + " :Not enough parameters");
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
    return true;
}

static bool validateKey(Server &server, Client &client, const std::vector<std::string> &params, Channel *channel)
{
    if (channel->getHasKey())
    {
        // missing key or wrong key
        if (params.size() <= 2 || channel->getKey() != params[2])
        {
            sendNumeric(client, "475", server.getName(), channel->getName() + " :Cannot join channel (+k)");
            return false;
        }
    }
    return true;
}

static bool canJoinChannel(Server &server, Client &client, Channel *channel)
{
    if (channel->hasMember(client.getFd()))
    {
        sendNumeric(client, "443", server.getName(), channel->getName() + " :is already on channel");
        return false;
    }
    if (channel->getIsInviteOnly() && !channel->isinvited(client.getFd()))
    {
        sendNumeric(client, "473", server.getName(), channel->getName() + " :Cannot join channel (+i)");
        return false;
    }

    // better to make it always positive when setting user limit
    // for example if i set +l to 0 or less, it should store it as 1
    if (channel->getUserLimit() > 0 &&
        channel->getUserNumber() >= channel->getUserLimit())
    {
        // for testing
        std::cout << "user number " << channel->getUserNumber() << "user limit: " << channel->getUserLimit() << std::endl;
        sendNumeric(client, "471", server.getName(), channel->getName() + " :Cannot join channel (+l)");
        return false;
    }
    return true;
}

void Commands::handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!checkRegistered(server, client) || !validateParams(server, client, params, 2, "JOIN"))
        return;

    std::string chanName = params[1];
    if (!ValidateChannelName(chanName, server, client))
        return;

    Channel *channel = server.findChannel(chanName);

    if (!channel)
        channel = server.createChannel(chanName, client);
    else
    {
        if (!validateKey(server, client, params, channel) || !canJoinChannel(server, client, channel))
            return;
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
