#include "../includes/Commands.hpp"

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

static bool sendToChannel(Server &server, Client &client, std::string message, std::string target)
{
    Channel *channel = server.findChannel(target);
    if (!channel)
    {
        sendNumeric(client, "403", server.getName(), target + " :No such channel");
        return false;
    }
    if (!channel->hasMember(client.getFd()))
    {
        sendNumeric(client, "442", server.getName(), target + " :You're not on that channel");
        return false;
    }
    std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
    channel->broadcastExcept(client.getFd(), fullMsg);
    return true ;
}

static bool sendMsgToClient(Server &server, Client &client, std::string message, std::string target)
{
    Client *recipient = server.getClientFromNickname(target);
    if (!recipient)
    {
        sendNumeric(client, "401", server.getName(), target + " :No such nick");
        return false;
    }
    std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " + recipient->getNickname() + " :" + message + "\r\n";
    send(recipient->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
    return true ;
}

void Commands::handlePrivmsg(Server &server,
                             Client &client,
                             const std::vector<std::string> &params)
{
    if (!checkRegistered(server, client) || validateParams(server, client, params, 3))
        return;

    std::string message = concatinate_params(params, 2);
    if (message.empty())
    {
        sendNumeric(client, "412", server.getName(), ":No text to send");
        return;
    }

    std::string target = params[1];
    if (target[0] == '#')
    {
        if (!sendToChannel(server, client, message, target))
            return ;
    }
    else if (!sendMsgToClient(server, client, message, target))
        return ;
}

void Commands::handlePart(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!validateParams(server, client, params, 2))
        return;

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
    channel->decrementUserNumber();
}

void Commands::handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!validateParams(server, client, params, 2))
        return;

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
    if (params.size() == 2)
    {
        std::string message = ":" + client.getNickname() + "!" + server.getName() + " :" + channel->getTopic() + "\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
    }
    else
    {
        // check if input starts with a :
        if (channel->getIsTopicRestricted() && !channel->isOperator(client.getFd()))
        {
            sendNumeric(client, "482", server.getName(), channel->getName() + " :You're not an operator");
        }
        if (params[2].size() > 0)
        {
            std::string topic = concatinate_params(params, 2);
            if (topic[0] != ':')
            {
                //change error code
                sendNumeric(client, "numeric", server.getName(), channel->getName() + " :bad topic it doesnt start with :");
                return ;
            }
            topic = topic.substr(1);
            channel->setTopic(topic);
        }
    }
}

// KICK #channel nickname :optional reason
void Commands::handleKick(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!validateParams(server, client, params, 3))
        return;

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
    if (!validateParams(server, client, params, 3))
        return;

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
    if (channel->getIsInviteOnly())
    {
        if (!channel->isOperator(client.getFd()))
        {
            sendNumeric(client, "473", server.getName(), params[1] + " :not operator cannot invote Cannot join channel (+i)");
            //delete possible member?
            return;
        }
    }
    // should we broadcast a message here?
    channel->addToInvitedMembersList(possibleMember->getFd());
    // channel->addMember(possibleMember);
}
