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
    this->name = "ft_irc_server";

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

std::string Server::getName() const
{
    return name;
}

bool Server::isNicknameTaken(const std::string &nick)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == nick)
            return true;
    }
    return false;
}

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
                {
                    acceptClient();
                    // std::cout<<"in accept client message"<<std::endl;
                }
                else
                {
                    // needs more handling later
                    // std::cout<<"in handle client message"<<std::endl;
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

std::string Server::getCreationDate() const
{
    char buffer[128];
    std::time_t creationTime = std::time(NULL);
    std::tm *timeinfo = std::localtime(&creationTime);

    std::strftime(buffer, sizeof(buffer),
                  "%a %b %d %H:%M:%S %Y", timeinfo);

    return std::string(buffer);
}

std::string Server::getPassword() const
{
    return password;
}

void Server::acceptClient()
{
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(serverFd, (sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        // in non-blocking mode, accept can return EWOULDBLOCK or EAGAIN if there are no pending connections
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            perror("accept");
        return;
    }

    set_non_blocking(client_fd);

    pollfd client_poll = {};
    client_poll.fd = client_fd;
    client_poll.events = POLLIN;
    pollFds.push_back(client_poll);

    clients.insert(std::make_pair(client_fd, Client(client_fd)));
    std::cout << "New client connected: " << client_fd << std::endl;
}
void Server::removeClient(int fd)
{
    std::map<int, Client>::iterator it = clients.find(fd);
    if (it == clients.end())
        return;

    Client *client = &it->second;

    // Remove from all channels
    for (std::map<std::string, Channel>::iterator ch = channels.begin();
         ch != channels.end();)
    {
        ch->second.removeMember(client);

        if (ch->second.getMembers().empty())
        {
            std::map<std::string, Channel>::iterator toDelete = ch;
            ++ch;
            channels.erase(toDelete);
        }
        else
        {
            ++ch;
        }
    }

    // Remove from pollfds
    for (size_t i = 0; i < pollFds.size(); ++i)
    {
        if (pollFds[i].fd == fd)
        {
            pollFds.erase(pollFds.begin() + i);
            break;
        }
    }

    close(fd);
    clients.erase(it);
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
    // client closed connection
    if (bytes == 0)
    {
        removeClient(fd);
        return;
    }
    if (bytes < 0)
    {
        // in non-blocking mode, recv can return EWOULDBLOCK or EAGAIN if there's no data to read
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            removeClient(fd);
        return;
    }

    it->second.appendBuffer(std::string(buffer, bytes));

    // while (it->second.hasCompleteCommand())
    // {
    //     std::string cmd = it->second.extractCommand();
    //     executeCommand(it->second, cmd);
    //     if (!it)
    //         break;
    // }
    while (true)
    {
        it = clients.find(fd);
        if (it == clients.end())
            return ;
        if (!it->second.hasCompleteCommand())
            return ;
        std::string cmd = it->second.extractCommand();
        executeCommand(it->second, cmd);
    }
}

void Server::executeCommand(Client &client, std::string command)
{
    Commands commands;
    commands.execute(*this, client, command);
}

std::map<std::string, Channel> &Server::getChannels()
{
    return (this->channels);
}

Channel *Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel>::iterator it = channels.find(name);
    if (it == channels.end())
        return 0;
    return &it->second;
}

Client *Server::getClientFromNickname(std::string nickname)
{
    std::map<int, Client>::iterator it;

    for (it = clients.begin(); it != clients.end(); ++it)
        if (it->second.getNickname() == nickname)
            return (&it->second);
    return (NULL);
}

Channel* Server::createChannel(const std::string &name, Client &creator)
{
    std::pair<std::map<std::string, Channel>::iterator, bool> result;
    result = channels.insert(std::make_pair(name, Channel(name)));

    if (!result.second)
        return NULL; // insertion failed

    Channel &chan = result.first->second;

    chan.addMember(&creator);
    chan.addOperator(creator.getFd());
    chan.incrementUserNumber();

    // should delete later, it's just for testing
    std::cout << "Channel created: " << name << " by " << creator.getNickname() << std::endl;

    return &chan;
}
