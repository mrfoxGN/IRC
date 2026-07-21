#include "../server/Server.hpp"

void Server::handleNick(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	std::string nick;
	iss >> nick;

	if (!client.isPassOk())
	{
		client.sendMsg(":localhost 464 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Password incorrect or not provided\r\n");
		return;
	}

	if (nick.empty())
	{
		client.sendMsg(":localhost 431 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :No nickname given\r\n");
		return;
	}

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nick)
		{
			client.sendMsg(":localhost 433 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " " + nick + " :Nickname is already in use\r\n");
			return;
		}
	}

	std::string oldNick = client.getNickname();
	client.setNickname(nick);
	client.setHasNick(true);

	if (client.isRegistered())
	{
		client.sendMsg(":" + oldNick + "!" + client.getUsername() + "@localhost NICK :" + nick + "\r\n");
	}
	else if (!client.hasUser())
	{
		// Wait for USER command silently as per IRC protocol
	}
	else
	{
		client.setRegistered(true);
		client.sendMsg(":localhost 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + client.getUsername() + "@localhost\r\n");
	}
}
