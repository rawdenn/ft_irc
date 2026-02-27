// socket fd
// nickname
// username
// realname
// registered state
// buffer

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.h"

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
    bool welcomeSent;
    bool sentPass;
    bool sentNick;
    bool sentUser;

public:
    Client(int fd);
    ~Client();

    int getFd() const;
    bool isRegistered() const;
    bool isPassAccepted() const;
    bool isWelcomeSent() const;
    bool hasSentPass() const;
    bool hasSentNick() const;
    bool hasSentUser() const;

    const std::string &getNickname() const;
    const std::string &getUsername() const;
    const std::string &getRealname() const;
    const std::string getPrefix() const;

    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setRealname(const std::string &real);
    void setPassAccepted(bool val);
    void setRegistered(bool val);
    void setWelcomeSent(bool val);
    void setHasSentPass(bool val);
    void setHasSentNick(bool val);
    void setHasSentUser(bool val);
    void appendBuffer(const std::string &data);
    bool hasCompleteCommand() const;
    std::string extractCommand();
};

#endif