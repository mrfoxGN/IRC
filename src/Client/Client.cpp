#include "Client.hpp"
#include <sys/socket.h>

Client::Client(int fd)
    : _fd(fd),
      _nickname("*"),
      _username(""),
      _realname(""),
      _buffer(""),
      _registered(false),
      _passOk(false),
      _hasNick(false),
      _hasUser(false)
{}

Client::~Client() {}

// ─── Getters

int                Client::getFd() const         { return _fd; }
const std::string& Client::getNickname() const   { return _nickname; }
const std::string& Client::getUsername() const   { return _username; }
const std::string& Client::getRealname() const   { return _realname; }
std::string&       Client::getBuffer()           { return _buffer; }
bool               Client::isRegistered() const  { return _registered; }
bool               Client::isPassOk() const      { return _passOk; }
bool               Client::hasNick() const       { return _hasNick; }
bool               Client::hasUser() const       { return _hasUser; }

// ─── Setters

void Client::setNickname(const std::string& nick) { _nickname = nick; _hasNick = true; }
void Client::setUsername(const std::string& user) { _username = user; }
void Client::setRealname(const std::string& real) { _realname = real; _hasUser = true; }
void Client::setPassOk(bool val)                  { _passOk = val; }
void Client::setHasNick(bool val)                 { _hasNick = val; }
void Client::setHasUser(bool val)                 { _hasUser = val; }
void Client::setRegistered(bool val)              { _registered = val; }

// ─── Buffer

void Client::appendToBuffer(const std::string& data) {
    _buffer += data;
}

void Client::clearBuffer() {
    _buffer.clear();
}

// ─── Send

void Client::sendMsg(const std::string& message) const {
    send(_fd, message.c_str(), message.size(), 0);
}

// ─── Prefix

std::string Client::getPrefix() const {
    return _nickname + "!" + _username + "@localhost";
}