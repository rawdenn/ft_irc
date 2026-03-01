#include "../includes/Commands.hpp"

// i: Set/remove Invite-only channel : WORKS
// t: Set/remove the restrictions of the TOPIC command to channel operators : WORKS
// k: Set/remove the channel key (password) : WORKS
// o: Give/take channel operator privilege : WORKS
// l: Set/remove the user limit to channel : WORKS

void parseModesUserLimit(Client &client, Channel *channel, Server &server, const std::vector<std::string> &params, size_t *params_index, bool sign)
{
    if (!sign)
    {
        sendNumeric(client, "324", server.getName(), "-l");
        channel->setUserLimit(-1);
        return ;
    }
    int userLimit = atoi(params[*params_index].c_str());
    if (userLimit < 0)
    {
        //change error message
        sendNumeric(client, "461", server.getName(), "MODE :Not enough parameters");
        return ;
    }
    channel->setUserLimit(userLimit);
    std::string modeMsg = "+l ";
    modeMsg += params[*params_index];
    (*params_index)++;
    sendNumeric(client, "324", server.getName(), modeMsg);
}

static bool checkModeParam(Server &server, Client &client, const std::vector<std::string> &params, size_t params_index)
{
    if (params_index > params.size())
    {
        sendNumeric(client, "461", server.getName(), " :MODE:Not enough parameters");
        return false;
    }
    return true;
}

static void handleModesOperator(Channel *channel, size_t *params_index, bool sign,
    const std::vector<std::string> &params, Server &server, Client &client)
{
    if (!checkModeParam(server, client, params, *params_index))
        return ;
    Client *potentialMember = server.getClientFromNickname(params[*params_index]);
    if (!potentialMember)
    {
        //change message
        sendNumeric(client, "401", server.getName(), " :No such nick");
        return ;
    }
    if (!channel->hasMember(potentialMember->getFd()))
    {
        sendNumeric(client, "441", server.getName(), potentialMember->getNickname() + " " + channel->getName() + " :They aren't on that channel");
        return;
    }

    std::string modeMsg = "";
    if (sign)
    {
        channel->addOperator(potentialMember->getFd());
        modeMsg = "+o ";

    }
    else
    {
        channel->removeOperator(potentialMember->getFd());
        modeMsg = "-o ";
    }
    modeMsg += potentialMember->getNickname();
    sendNumeric(client, "324", server.getName(), modeMsg);
    (*params_index)++;
    std::cout << "Mode " << sign << "o (operator)\n";
}

static void parseModesKey(Channel *channel, const std::vector<std::string> &params, size_t *params_index, bool sign)
{
    if (sign == true)
    {
        if (*params_index <params.size())
        {
            channel->setKey(params[*params_index]);
            (*params_index)++;
        }
    }
    else
        channel->setKey("");
    channel->setHasKey(sign);
    std::cout << "Mode " << sign << "k (channel key)\n";
}

static bool parseSign(bool &sign, char c)
{
    if (c == '+')
    {
        sign = true;
        return true;
    }
    if (c == '-')
    {
        sign = false;
        return true;
    }
    return (false);
}

static void parseModeNoParams(char mode, bool sign, Channel *channel)
{
    switch (mode)
    {
        case 'i':
            channel->setIsInviteOnly(sign);
            // std::cout<<"set as invite only\n";
            break;
        case 't':
            channel->setIsTopicRestricted(sign);
            break;
        default:
        // Invalid mode character
        std::cout << "ERR_UNKNOWNMODE: '" << mode << "' is unknown mode char\n";
        break;
    }
}

static void parseModeParams(char mode, bool sign, Channel *channel, Server &server,
        Client &client, const std::vector<std::string> &params, size_t &params_index)
{
    switch (mode)
    {
        case 'k':
            parseModesKey(channel, params, &params_index, sign);
            break;
        case 'o':
        {
            handleModesOperator(channel, &params_index, sign, params, server, client);
            break;
        }
        case 'l':
        
            if (!checkModeParam(server, client, params, params_index))
                break ;
            parseModesUserLimit(client, channel, server, params, &params_index, sign);
            break;
        default:
            // Invalid mode character
            std::cout << "ERR_UNKNOWNMODE: '" << mode << "' is unknown mode char\n";
            break;
    }
}

void parseModes(Server &server, Client &client, Channel *channel, const std::vector<std::string> &params)
{
    bool        sign = false;
    char        c;
    std::string modes = params[2];
    size_t      params_index = 3;

    for (size_t i = 0; i < modes.length(); i++)
    {
        c = modes[i];
        if (parseSign(sign, c))
            continue ;
        if (c == 'i' || c == 't')
        {
            parseModeNoParams(c, sign, channel);
        }
        else if (c == 'o' || c == 'l' || c == 'k')
            parseModeParams(c, sign, channel, server, client, params, params_index);
        else
        std::cout << "ERR_UNKNOWNMODE: '" << c << "' is unknown mode char\n"; // Invalid mode character
    }
}

void Commands::handleMode(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (!checkRegistered(server, client) || !validateParams(server, client, params, 3, "MODE"))
        return;

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
    if (!channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "482", server.getName(), params[1] + " :not an operator");
        return ;
    }
    parseModes(server, client, channel, params);
}
