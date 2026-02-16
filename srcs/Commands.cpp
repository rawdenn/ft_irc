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
    cmdMap["PASS"]    = &Commands::handlePass;
    cmdMap["NICK"]    = &Commands::handleNick;
    cmdMap["USER"]    = &Commands::handleUser;
    cmdMap["QUIT"]    = &Commands::handleQuit;
    cmdMap["JOIN"]    = &Commands::handleJoin;
    cmdMap["PRIVMSG"] = &Commands::handlePrivmsg;
    cmdMap["TOPIC"] = &Commands::handleTopic;
    // cmdMap["PART"]    = &Commands::handlePart;
    //not implemented yet
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
    if (client.hasSentPass || params.size() > 2)
        return; //plus some error message(probably best to split the 2 checks)

    // if wrong password, disconnect client
    if (params[1] != server.getPassword())
    {
        sendNumeric(client, "464", server.getName(), "Password incorrect");
        // server.removeClient(client.getFd()); //client should be able to try again not be disconnected
        return;
    }

    client.hasSentPass = true;
}

void Commands::handleNick(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "431", server.getName(), "No nickname given");
        return;
    }

    std::string nickname = params[1];
    if (server.isNicknameTaken(nickname))
    {
        sendNumeric(client, "433", server.getName(), nickname + " Nickname is already in use");
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
}

void Commands::handleQuit(Server &server, Client &client, const std::vector<std::string> &params)
{
    (void)params;
    server.removeClient(client.getFd());
    //TO-DO: should we add a different error message? 
}

void Commands::handleJoin(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    { 
        //print error not enough params
        return ;
    }

    if (params[1].c_str()[0] != '#')
    {
        //print error bad channel name
        return ;
    }

    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        Channel newChannel(params[1]);
        newChannel.addMember(&client);
        newChannel.addOperator(client.getFd());
        server.getChannels().insert(make_pair(params[1], newChannel));
        std::string message = ":" + client.getNickname() + "!" + server.getName()
                                + " JOIN " + newChannel.getName() + "\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
    }
    else 
    {
        channel->addMember(&client);

        std::string message = ":" + client.getNickname() + "!" + server.getName()
                                + " JOIN " + channel->getName() + "\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
        
        message = ":" + server.getName() + "[THE RPL NUM] " + client.getNickname()
                    + " " + channel->getName() + ":" + channel->getTopic() + "\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
        
        //loop through list of clients and print them
        message = ":" + server.getName() + "[THE RPL NUM] " + client.getNickname()
                    + " " + channel->getName() + ":" + "list of names\n";
        send(client.getFd(), message.c_str(), message.size(), 0);

        message = ":" + server.getName() + "[THE RPL NUM] " + client.getNickname()
                    + " " + channel->getName() + ":" + "End of Names list\n";
        send(client.getFd(), message.c_str(), message.size(), 0);
    }
    
}

void Commands::handlePrivmsg(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 3 )
    {
        //print no such channel exists 
        return ; //TO-DO: add error message
    }
    
    if (params[1].c_str()[0] != '#')
    {
        //print bad channel name
        return ;
    }

    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        std::cerr<<"channel does not exist\n"<<std::endl; //check if err message on cerr or if send or if throw exception
        return ;
    }
    std::string message ="";
    for (size_t i = 2; i < params.size(); i++)
    {
        message += params[i];
        if (i + 1 != params.size())
            message += " ";
    }
    std::string fullMsg = ":" + client.getNickname() + "!" + server.getName()
            + " PRIVMSG" + " " + channel->getName() + " :" + message + "\r\n";
    channel->broadcast(client.getFd(), fullMsg);
}

// void Commands::handlePart(Server &server, Client &client, const std::vector<std::string> &params)
// {   
//     //is this command required?   
// }

void Commands::handleTopic(Server &server, Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2 )
    {
        //print error 
        return ; //TO-DO: add error message
    }
    
    if (params[1].c_str()[0] != '#')
    {
        //print bad channel name
        return ;
    }
    Channel *channel = server.findChannel(params[1]);
    if (channel == 0)
    {
        std::cerr<<"channel does not exist\n"<<std::endl; //check if err message on cerr or if send or if throw exception
        return ;
    }
    if (params.size() == 2)
    {
        if (channel->getTopic() == "")
        {
            std::string message = ":" + client.getNickname() + "!" + server.getName()
                                    + " :" + channel->getTopic() + "\n";
            send(client.getFd(), message.c_str(), message.size(), 0);
        }
        else
        {
            std::string message = ":" + client.getNickname() + "!" + server.getName()
                                    + " :" + channel->getTopic() + "\n";
            send(client.getFd(), message.c_str(), message.size(), 0);
        }
    }
    else
    {
        //check if input starts with a :
        //check if topic can be more than 1 word. if yes concatinate the sentence in 1 and set it
        if (channel->isOperator(client.getFd()))
            channel->setTopic(params[3]);
        else
            std::cout<<"youre not an op\n"; //change this message
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
        //error message unknown command
    }
    
    if (client.isRegistered() && !client.isWelcomeSent())
    {
        sendWelcome(server, client);
        std::cout << "Client " << client.getNickname() << " is now registered." << std::endl;
        client.setWelcomeSent(true);
    }

}
