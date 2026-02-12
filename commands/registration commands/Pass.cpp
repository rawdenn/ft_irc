#include "../includes/Server.hpp"

//TO-DO: to handle if input more than 1 word. unsure if here ro somewhere before
void _Pass(std::string password, Client *client, Server *server)
{
    if (!client->isRegistered())
    {
        if(password == server->getPassword())
        {
            client.setPassAccepted(true);
        }
    }

}