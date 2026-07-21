#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _buffer;
    bool        _registered;
    bool        _passOk;
    bool        _hasNick;
    bool        _hasUser;

public:
    Client(int fd);
    ~Client();

    // Getters
    int                 getFd() const;
    const std::string&  getNickname() const;
    const std::string&  getUsername() const;
    const std::string&  getRealname() const;
    std::string&        getBuffer();
    bool                isRegistered() const;
    bool                isPassOk() const;
    bool                hasNick() const;
    bool                hasUser() const;
    
    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setPassOk(bool val);
    void setHasNick(bool val);
    void setHasUser(bool val);
    void setRegistered(bool val);

      // Buffer management
    void        appendToBuffer(const std::string& data);
    void        clearBuffer();

    // Send a message to this client
    void        sendMsg(const std::string& message) const;

    // Helper — IRC prefix format: nick!user@host
    std::string getPrefix() const;
    
};

#endif