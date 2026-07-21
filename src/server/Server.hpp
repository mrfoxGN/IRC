#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>	// For htons
#include <fcntl.h>		// For fcntl and O_NONBLOCK
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <cctype>
#include <sstream>
#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"

#define MAX_EVENTS 10

using namespace std;

class Server
{
private:
	bool _Run_state;
	int _listen_fd;
	int _epoll_fd;

	int _port;
	string _password;
	std::map<int, Client> _clients;
	std::map<std::string, Channel> _channel;

	// Private helper methods
	void processClientBuffer(Client &client);
	void handleCommand(Client &client, const std::string &line);

	// Command Handlers
	void handlePass(Client &client, const std::string &line);
	void handleNick(Client &client, const std::string &line);
	void handleUser(Client &client, const std::string &line);
	void handleJoin(Client &client, const std::string &line);
	void handlePart(Client &client, const std::string &line);
	void handlePrivmsg(Client &client, const std::string &line);
	void handleTopic(Client &client, const std::string &line);
	void handleInvite(Client &client, const std::string &line);
	void handleKick(Client &client, const std::string &line);
	void handleMode(Client &client, const std::string &line);
    void handleMan(Client &client, const std::string &line);
    void joinChannel(Client &client, const std::string &channel, const std::string &key);


public:
	Server();
	Server(int Port, string Password);
	void run(int &running);
	~Server();

	class My_Exception : public exception
	{

	private:
		string _msg;

	public:
		My_Exception(string msg);
		const char *what() const throw();
		virtual ~My_Exception() throw();
	};
};

#endif
