#include "Server.hpp"
#include "../Channel/Channel.hpp"
#include "../Client/Client.hpp"
#include <cstring>

Server::My_Exception::My_Exception(string msg) : _msg(msg) {}
const char *Server::My_Exception::what() const throw()
{
	return _msg.c_str();
}
Server::My_Exception::~My_Exception() throw() {}
Server::Server(int Port, string Password)
{
	this->_port = Port;
	this->_password = Password;

	this->_listen_fd = -1;
	this->_epoll_fd = -1;

	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
	{
		throw(My_Exception("SOCKET FAIL"));
	}

	int opt = 1;
	if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_listen_fd);
		throw(My_Exception("SETSOCKOPT FAIL"));
	}

	if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(_listen_fd);
		throw(My_Exception("FCNTL FAIL"));
	}

	struct sockaddr_in server_adr;
	memset(&server_adr, 0, sizeof(server_adr));
	server_adr.sin_family = AF_INET;
	server_adr.sin_addr.s_addr = INADDR_ANY;
	server_adr.sin_port = htons(this->_port);

	if (bind(_listen_fd, (struct sockaddr *)&server_adr, sizeof(server_adr)) < 0)
	{
		close(_listen_fd);
		throw(My_Exception("BIND FAIL"));
	}

	if (listen(_listen_fd, SOMAXCONN) < 0)
	{
		close(_listen_fd);
		throw(My_Exception("LISTEN FAIL"));
	}
}

void Server::run(int &running)
{
	struct sockaddr_in client_adr;
	socklen_t client_adr_len = sizeof(client_adr);
	int client_fd;
	struct epoll_event ev, events[MAX_EVENTS];

	_epoll_fd = epoll_create(1);
	if (_epoll_fd < 0)
		throw(My_Exception("EPOLL_CREATE FAIL"));

	ev.events = EPOLLIN;
	ev.data.fd = _listen_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listen_fd, &ev) < 0)
		throw(My_Exception("EPOLL_CTL FAIL"));

	this->_Run_state = true;

	cout << "Server listening on port " << _port << "..." << endl;

	while (_Run_state && running)
	{
		int ret = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			throw(My_Exception("epoll_wait fail"));
		}

		for (int i = 0; i < ret; i++)
		{
			if (events[i].data.fd == _listen_fd)
			{
				client_adr_len = sizeof(client_adr);
				client_fd = accept(_listen_fd, (struct sockaddr *)&client_adr, &client_adr_len);
				if (client_fd < 0)
				{
					cerr << "accept() failed" << endl;
					continue;
				}

				if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
				{
					cerr << "fcntl() failed on client socket" << endl;
					close(client_fd);
					continue;
				}

				cout << "Client connected from " << inet_ntoa(client_adr.sin_addr)
					 << ":" << ntohs(client_adr.sin_port) << endl;
				_clients.insert(std::make_pair(client_fd, Client(client_fd)));
				
				ev.events = EPOLLIN;
				ev.data.fd = client_fd;
				epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
			}
			else if (events[i].events & EPOLLIN)
			{
				int current_fd = events[i].data.fd;
				char buffer[1024];
				int bytes = recv(current_fd, buffer, sizeof(buffer) - 1, 0);

				if (bytes <= 0)
				{
					cout << "Client disconnected (FD: " << current_fd << ")" << endl;
					epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
					close(current_fd);
					if (_clients.find(current_fd) != _clients.end())
					{
						_clients.erase(current_fd);
					}
					continue;
				}
				buffer[bytes] = '\0';
				std::map<int, Client>::iterator it = _clients.find(current_fd);
				if (it != _clients.end())
				{
					it->second.appendToBuffer(std::string(buffer, bytes));
					processClientBuffer(it->second);
				}
			}
		}
	}
}

Server::~Server()
{
	if (_epoll_fd >= 0)
		close(_epoll_fd);
	if (_listen_fd >= 0)
		close(_listen_fd);
	_clients.clear();
}

void Server::processClientBuffer(Client &client)
{
	std::string &buf = client.getBuffer();

	size_t pos;
	while ((pos = buf.find('\n')) != std::string::npos)
	{
		std::string line = buf.substr(0, pos);
		buf.erase(0, pos + 1);

		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		if (!line.empty())
			handleCommand(client, line);
	}
}

void Server::handleCommand(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	
	if (!(iss >> command))
		return;

	for (size_t i = 0; i < command.size(); i++)
		command[i] = toupper(command[i]);

	if (command == "PASS")
		handlePass(client, line);
	else if (command == "NICK")
		handleNick(client, line);
	else if (command == "USER")
		handleUser(client, line);
	else if (command == "JOIN")
		handleJoin(client, line);
	else if (command == "PART")
		handlePart(client, line);
	else if (command == "PRIVMSG")
		handlePrivmsg(client, line);
	else if (command == "MAN")
		handleMan(client, line);
	else if (command == "TOPIC")
		handleTopic(client, line);
	else if (command == "INVITE")
		handleInvite(client, line);
	else if (command == "KICK")
		handleKick(client, line);
	else if (command == "MODE")
		handleMode(client, line);
	else
	{
		client.sendMsg(":localhost 421 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " " + command + " :Unknown command\r\n");
	}
}
