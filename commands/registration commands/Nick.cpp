// NICK <nickname>

//     Must be unique

//     Can change nickname after registration

//     If duplicate → 433 ERR_NICKNAMEINUSE

//     If missing → 431 ERR_NONICKNAMEGIVEN

#include "../includes/Server.hpp"

// void _setNickName(std::string nickname, Client *client, Server *server)
// {
//     if (!client->isRegistered())
//     {
//         std::map<int, Client>::iterator it = clients.begin();
//         while ()
//     }

// }

    // if (it != clients.end())
    //     clients.erase(it);

    // for (size_t i = 0; i < pollFds.size(); ++i)
    // {
    //     if (pollFds[i].fd == fd)
    //     {
    //         pollFds.erase(pollFds.begin() + i);
    //         break;
    //     }
    // }

    // close(fd);
    // std::cout << "Client disconnected: " << fd << std::endl;