#include "../server/Server.hpp"
#include <sstream>
#include <cctype>


static bool isValidNick(const std::string &nick)
{
	if (nick.empty())
		return false;

	if (!std::isalpha(nick[0]))
		return false;

	for (size_t i = 0; i < nick.size(); i++)
	{
		char c = nick[i];

		if (!std::isalnum(c) &&
			c != '-' &&
			c != '[' &&
			c != ']' &&
			c != '\\' &&
			c != '`' &&
			c != '^' &&
			c != '_')
			return false;
	}

	return true;
}

void Server::handleNick(Client &client, const std::string &line)
{
	std::istringstream iss(line);

	std::string cmd;
	std::string nick;

	iss >> cmd >> nick;

	if (!client.isPassOk())
	{
		client.sendMsg(":localhost 464 " +
			(client.getNickname().empty() ? "*" : client.getNickname()) +
			" :Password incorrect or not provided\r\n");
		return;
	}

	if (nick.empty())
	{
		client.sendMsg(":localhost 431 " +
			(client.getNickname().empty() ? "*" : client.getNickname()) +
			" :No nickname given\r\n");
		return;
	}


	if (!isValidNick(nick))
	{
		client.sendMsg(":localhost 432 " +
			(client.getNickname().empty() ? "*" : client.getNickname()) +
			" " + nick +
			" :Erroneous nickname\r\n");
		return;
	}

	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
	{
		if (&it->second == &client)
			continue;

		if (it->second.getNickname() == nick)
		{
			client.sendMsg(":localhost 433 " +
				(client.getNickname().empty() ? "*" : client.getNickname()) +
				" " + nick +
				" :Nickname is already in use\r\n");
			return;
		}
	}


	std::string oldNick = client.getNickname();

	client.setNickname(nick);
	client.setHasNick(true);


	if (client.isRegistered())
	{
		if (!oldNick.empty() && oldNick != nick)
		{
			std::string msg = ":" + oldNick +
			"!" + client.getUsername() +
			"@localhost NICK :" +
			nick + "\r\n";

			for (std::map<std::string, Channel>::iterator it = _channel.begin();
			 it != _channel.end(); ++it)
			{
				Channel &channel = it->second;

				if (channel.isMember(client.getFd()))
				{
					channel.broadcast(msg, client.getFd());
				}
			}

			client.sendMsg(msg);
		}

		return;
	}


	if (client.hasUser())
	{
		client.setRegistered(true);

		client.sendMsg(
			":localhost 001 " +
			nick +
			" :Welcome to the Internet Relay Network " +
			nick +
			"!" +
			client.getUsername() +
			"@localhost\r\n"
		);
	}
}