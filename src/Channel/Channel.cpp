#include "Channel.hpp"
#include "../Client/Client.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string& name)
    : _name(name),
      _topic(""),
      _inviteOnly(false),
      _topicRestricted(false),
      _hasPassword(false),
      _password(""),
      _hasUserLimit(false),
      _userLimit(0)
{}

Channel::~Channel() {}

// getters

const std::string& Channel::getName() const        { return _name; }
const std::string& Channel::getTopic() const       { return _topic; }
const std::string& Channel::getPassword() const    { return _password; }
int                Channel::getUserLimit() const   { return _userLimit; }
int                Channel::getMemberCount() const { return _members.size(); }


const std::map<int, Client*>& Channel::getMembers() const   { return _members; }
const std::map<int, Client*>& Channel::getOperators() const { return _operators; }

bool Channel::isInviteOnly() const      { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
bool Channel::hasPassword() const       { return _hasPassword; }
bool Channel::hasUserLimit() const      { return _hasUserLimit; }

//setters

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

void Channel::setInviteOnly(bool val) {
    _inviteOnly = val;
}

void Channel::setTopicRestricted(bool val) {
    _topicRestricted = val;
}

void Channel::setPassword(const std::string& password) {
    _password = password;
    _hasPassword = true;
}

void Channel::removePassword() {
    _password = "";
    _hasPassword = false;
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
    _hasUserLimit = true;
}

void Channel::removeUserLimit() {
    _userLimit = 0;
    _hasUserLimit = false;
}

 //invite list

 void Channel::addInvite(int fd) {
    // avoid duplicates
    if (!isInvited(fd))
        _inviteList.push_back(fd);
}

bool Channel::isInvited(int fd) const {
    for (size_t i = 0; i < _inviteList.size(); i++)
        if (_inviteList[i] == fd)
            return true;
    return false;
}

void Channel::removeInvite(int fd) {
    for (size_t i = 0; i < _inviteList.size(); i++) {
        if (_inviteList[i] == fd) {
            _inviteList.erase(_inviteList.begin() + i);
            return;
        }
    }
}

// member management

void Channel::addMember(Client* client) {
    _members[client->getFd()] = client;
}

void Channel::removeMember(int fd) {
    _members.erase(fd);
    _operators.erase(fd);  // if they were op, remove that too
}

bool Channel::isMember(int fd) const {
    return _members.find(fd) != _members.end();
}

// op management

void Channel::addOperator(Client* client) {
    _operators[client->getFd()] = client;
}

void Channel::removeOperator(int fd) {
    _operators.erase(fd);
}

bool Channel::isOperator(int fd) const {
    return _operators.find(fd) != _operators.end();
}

//broadcast

void Channel::broadcast(const std::string& message, int excludeFd) {
    std::map<int, Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it) {
        if (it->first != excludeFd)
            send(it->first, message.c_str(), message.size(), 0);
    }
}
