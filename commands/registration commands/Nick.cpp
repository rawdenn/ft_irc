// NICK <nickname>

//     Must be unique

//     Can change nickname after registration

//     If duplicate → 433 ERR_NICKNAMEINUSE

//     If missing → 431 ERR_NONICKNAMEGIVEN

#include "../includes/Server.hpp"

void _setNickName(std::string nickname, Client *client, Server *server)
{
    //TO-DO (maybe?): check if nickname is 1 word or not
    if (!client->isRegistered())
    {
        if(server->isNicknameTaken(nickname) )
        {
            std::cout<<"Nickname taken, enter a different one\n"<<std::endl;
            return ;
        }
        client->setNickname(nickname);

    }

}
