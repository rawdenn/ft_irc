#include "Server.hpp"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

int main(int ac, char const **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return 1;
    }
    return 0;
}
