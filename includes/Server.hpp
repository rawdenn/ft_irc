// Creates socket
// bind()
// listen()
// accept()
// poll() loop
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdio>
#include <cerrno>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
private:
    int serverFd;
    int port;
    std::string name;
    std::string password;
    
    std::vector<pollfd> pollFds;
    std::map<int, Client> clients;           // key is fd
    std::map<std::string, Channel> channels; // key is channel name
    bool running; // for clean shutdown

public:
    Server(int port, const std::string &password);
    ~Server();

    void run();
    void shutdown();
    std::string getPassword() const;
    std::string getName() const;
    bool isNicknameTaken(const std::string &nick);

    // Client management
    void acceptClient();
    void removeClient(int fd);

    // Command handling
    void handleClientMessage(int fd);
    void executeCommand(Client &client, std::string command);
    // void broadcastMessage(const std::string& message, int excludeFd = -1);

    // Channel management
    Channel *findChannel(const std::string &name);
};

#endif

/* Some Notes
   ----------
   *this is how pollfd is defined in <poll.h>:
    struct pollfd {
        int   fd;        // the file descriptor (socket)
        short events;    // what we WANT to watch
        short revents;   // what ACTUALLY happened
    };

    *POLLIN means there's data to read
     POLLOUT means we can write without blocking
     POLLERR means an error occurred, etc.

    *Blocking means a function stops the program and waits until something happens.
     For example, accept() waits for a new connection, and recv() waits for data.
     While waiting, the whole server is frozen.

    *Non-blocking means the function returns immediately if it can’t do the operation.
     Instead of waiting, it gives an error like EWOULDBLOCK, allowing the server to continue running.

    *In our IRC server, we:
     Set sockets to non-blocking mode using fcntl()
     Use poll() to check which sockets are ready
     Only call accept() or recv() when data is available
     This prevents the server from freezing and allows it to handle multiple clients at the same time.

*/