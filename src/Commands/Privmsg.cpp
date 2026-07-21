#include "../server/Server.hpp"
#include "../Bot/Bot.hpp"

Bot bot;

static std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::istringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

void Server::handlePrivmsg(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string cmd, target, message;

	iss >> cmd >> target;
	std::getline(iss >> std::ws, message);
	if(!message.empty() && message[0] == ':')
		message.erase(0, 1);

	if (!client.isPassOk())
	{
		client.sendMsg(":localhost 464 " + client.getNickname() + " :Password incorrect\r\n");
		return;
	}
	if (!client.isRegistered())
	{
		client.sendMsg(":localhost 451 " + client.getNickname() + " :You have not registered\r\n");
		return;
	}
	if (target.empty())
	{
		client.sendMsg(":localhost 461 " + client.getNickname() + " PRIVMSG :Not enough parameters\r\n");
		return;
	}
	if (message.empty())
	{
		client.sendMsg(":localhost 412 " + client.getNickname() + " :No text to send\r\n");
		return;
	}
	if (bot.isBotCommand(message)){
		std::string response = bot.execute(message);
		std::string botMsg = ":Bot PRIVMSG " + target +
							" :" + response + "\r\n";
		std::map<std::string, Channel>::iterator it = _channel.find(target);
		if (it != _channel.end())
			it->second.broadcast(botMsg, -1);

		return;
	}
	std::vector<std::string> targets = split(target, ',');

	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::string currentTarget = targets[i];
		std::string fullMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + currentTarget + " :" + message + "\r\n";

		if (currentTarget[0] == '#' || currentTarget[0] == '&')
		{
			if (_channel.find(currentTarget) == _channel.end())
			{
				client.sendMsg(":localhost 401 " + client.getNickname() + " " + currentTarget + " :No such nick/channel\r\n");
				continue;
			}

			Channel &ch = _channel.find(currentTarget)->second;

			if (!ch.isMember(client.getFd()))
			{
				client.sendMsg(":localhost 404 " + client.getNickname() + " " + currentTarget + " :Cannot send to channel\r\n");
				continue;
			}

			ch.broadcast(fullMsg, client.getFd());
		}
		else
		{
			bool found = false;
			for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				if (it->second.getNickname() == currentTarget)
				{
					it->second.sendMsg(fullMsg);
					found = true;
					break;
				}
			}

			if (!found)
			{
				client.sendMsg(":localhost 401 " + client.getNickname() + " " + currentTarget + " :No such nick/channel\r\n");
			}
		}
	}
}