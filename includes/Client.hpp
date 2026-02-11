// socket fd
// nickname
// username
// realname
// registered state
// buffer

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
class Client
{
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string buffer;
    bool registered;
    bool passAccepted;

public:
    Client(int fd);
    Client(int fd, const std::string& nick, const std::string& user, const std::string& real);
    ~Client();

    int getFd() const;
    bool isRegistered() const;
    bool isPassAccepted() const;

    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;

    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setPassAccepted(bool val);
    void setRegistered(bool val);

    void appendBuffer(const std::string& data);
    bool hasCompleteCommand() const;
    std::string extractCommand();
};

#endif