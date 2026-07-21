// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>

class Client;

class Channel {
private:
    std::string             _name;
    std::string             _topic;
    std::map<int, Client*>  _members;    // fd → Client*
    std::map<int, Client*>  _operators;  // fd → Client*
    std::vector<int>        _inviteList; // fds of invited clients

    // modes
    bool        _inviteOnly;   // +i
    bool        _topicRestricted; // +t
    bool        _hasPassword;  // +k
    std::string _password;
    bool        _hasUserLimit; // +l
    int         _userLimit;

public:
    Channel(const std::string& name);
    ~Channel();

    // Getters
    const std::string&  getName() const;
    const std::string&  getTopic() const;
    int                 getMemberCount() const;
    const std::string&  getPassword() const;
    int                 getUserLimit() const;

    // Mode getters
    bool isInviteOnly() const;
    bool isTopicRestricted() const;
    bool hasPassword() const;
    bool hasUserLimit() const;

    // Member management
    void addMember(Client* client);
    void removeMember(int fd);
    bool isMember(int fd) const;

    // Operator management
    void addOperator(Client* client);
    void removeOperator(int fd);
    bool isOperator(int fd) const;

    // Invite list
    void addInvite(int fd);
    bool isInvited(int fd) const;
    void removeInvite(int fd);

    // Setters
    void setTopic(const std::string& topic);
    void setInviteOnly(bool val);
    void setTopicRestricted(bool val);
    void setPassword(const std::string& password);
    void removePassword();
    void setUserLimit(int limit);
    void removeUserLimit();

    // Broadcast message to all members
    void broadcast(const std::string& message, int excludeFd = -1);

    // Get all members (needed for NAMES, JOIN replies)
    const std::map<int, Client*>& getMembers() const;
    const std::map<int, Client*>& getOperators() const;
};

#endif