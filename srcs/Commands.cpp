#include "../includes/Commands.hpp"

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

void Commands::execute(Server &server, Client &client, std::string &cmd)
{
    (void)server;
    std::vector<std::string> tokens = split(cmd, ' ');
    if (tokens.empty())
        return;

    std::string command = tokens[0];

    // TO-DO: replace this logic here later(after exec of all commands + adding them to a map)
    if (command == "PASS")
    {
        // later
    }
    else if (!client.isPassAccepted())
    {
        // later
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
