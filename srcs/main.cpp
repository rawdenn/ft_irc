#include "../includes/Server.hpp"
#include "../includes/ft_irc.h"

int main(int argc, char const **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    main_signal();
    try
    {
        int port = std::atoi(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
        while (g_running)
        {
            server.run();
        }
        server.shutdown();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
