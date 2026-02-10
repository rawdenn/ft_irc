#include "Commands.hpp"

//for example, split "USER username 0 * :Real Name" into ["USER", "username", "0", "*", ":Real Name"]
std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos)
    {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }

    // Add last token
    tokens.push_back(s.substr(start));
    return tokens;
}

void handleCommand(Server &server, Client &client, const std::string &cmd)
{
    std::vector<std::string> tokens = split(cmd, ' ');
    if (tokens.empty())
        return;

    std::string command = tokens[0];

    if (command == "PASS")
    {
        if (tokens.size() < 2)
        {
            // sendReply(client, "461 PASS :Not enough parameters");
            return;
        }

        if (tokens[1] == server.getPassword())
            client.setPassAccepted(true);
        else
        {
            // sendReply(client, "464 :Wrong password");
        }
    }
    else if (!client.isPassAccepted())
    {
        // sendReply(client, "451 :You have not registered");
    }
    else if (command == "NICK")
    {
        client.setNickname(tokens[1]);
    }
    else if (command == "USER")
    {
        client.setUsername(tokens[1]);
        client.setRealname(tokens.back());
        // Now client is registered
        if (client.getNickname() != "")
            client.setRegistered(true);
    }
}

