#include "../includes/Server.hpp"
#include "../includes/Commands.hpp"

// see notes in Server.hpp
static void set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return;
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Server::Server(int port, const std::string &password)
{
    // check if valid later
    this->password = password;
    this->port = port;
    this->running = false;

    // we create socket here...
    this->serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverFd < 0)
    {
        perror("socket");
        exit(1);
    }
    if (port)
    {
        // Allow reuse of port
        int opt = 1;
        setsockopt(this->serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }
}

Server::~Server()
{
    shutdown();
}

// this isnt final but just for testing connectivity on the port
void Server::run()
{
    // we bind and listen here...
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(serverFd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(serverFd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(1);
    }

    set_non_blocking(serverFd);

    // Add server socket to pollFds
    pollfd server_poll;
    server_poll.fd = serverFd;
    server_poll.events = POLLIN;
    pollFds.push_back(server_poll);

    running = true;
    std::cout << "Server started on port " << port << std::endl;

    while (running)
    {
        if (poll(&pollFds[0], pollFds.size(), -1) < 0)
        {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < pollFds.size(); i++)
        {
            if (pollFds[i].revents & POLLIN)
            {
                if (pollFds[i].fd == serverFd)
                    acceptClient();
                else
                {
                    // needs more handling later
                    handleClientMessage(pollFds[i].fd);
                }
            }
        }
    }
}

void Server::shutdown()
{
    if (!running)
        return;

    running = false;
    for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
        close(it->fd);
    pollFds.clear();
    clients.clear();
    channels.clear();
}

std::string Server::getPassword() const
{
    return password;
}

void Server::acceptClient()
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(serverFd, (sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        // in non-blocking mode, accept can return EWOULDBLOCK or EAGAIN if there are no pending connections
        // if (errno != EWOULDBLOCK && errno != EAGAIN)
            perror("accept");
        return;
    }

    set_non_blocking(client_fd);

    pollfd client_poll;
    client_poll.fd = client_fd;
    client_poll.events = POLLIN;
    pollFds.push_back(client_poll);

    clients.insert(std::make_pair(client_fd, Client(client_fd)));
    std::cout << "New client connected: " << client_fd << std::endl;
}

// should change after adding channels
void Server::removeClient(int fd)
{
    std::map<int, Client>::iterator it = clients.find(fd);
    if (it != clients.end())
        clients.erase(it);

    for (size_t i = 0; i < pollFds.size(); ++i)
    {
        if (pollFds[i].fd == fd)
        {
            pollFds.erase(pollFds.begin() + i);
            break;
        }
    }

    close(fd);
    std::cout << "Client disconnected: " << fd << std::endl;
}

void Server::handleClientMessage(int fd)
{
    std::map<int, Client>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes == 0)
    {
        // client closed connection
        removeClient(fd);
        return;
    }
    if (bytes < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            removeClient(fd);
        return;
    }

    it->second.appendBuffer(std::string(buffer, bytes));

    while (it->second.hasCompleteCommand())
    {
        std::string cmd = it->second.extractCommand();
        executeCommand(it->second, cmd);
    }
}

void Server::executeCommand(Client &client, std::string command)
{
    Commands commands;
    commands.execute(*this, client, command);
}

Channel *Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel>::iterator it = channels.find(name);
    if (it == channels.end())
        return 0;
    return &it->second;
}