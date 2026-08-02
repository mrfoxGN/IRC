#include "../server/Server.hpp"
#include <sstream>

void Server::handlePart(Client &client, const std::string &line)
{
	if (!client.isRegistered())
	{
		client.sendMsg(":localhost 451 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :You have not registered\r\n");
		return;
	}
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	std::string channelsStr;
	if (!(iss >> channelsStr))
	{
		client.sendMsg(":localhost 461 " + client.getNickname() + " PART :Not enough parameters\r\n");
		return;
	}

	std::string reason;
	std::string temp;
	if (iss >> temp)
	{
		if (temp[0] == ':')
		{
			reason = temp.substr(1);
			std::string rest;
			std::getline(iss, rest);
			reason += rest;
		}
		else
		{
			reason = temp;
			std::string rest;
			std::getline(iss, rest);
			if (!rest.empty())
				reason += rest;
		}
	}

	std::istringstream chIss(channelsStr);
	std::string channelName;

	while (std::getline(chIss, channelName, ','))
	{
		size_t start = channelName.find_first_not_of(" \t");
		size_t end = channelName.find_last_not_of(" \t");
		if (start == std::string::npos)
			continue;
		channelName = channelName.substr(start, end - start + 1);

		if (_channel.find(channelName) == _channel.end())
		{
			client.sendMsg(":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
			continue;
		}

		Channel &chan = _channel.find(channelName)->second;

		if (!chan.isMember(client.getFd()))
		{
			client.sendMsg(":localhost 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n");
			continue;
		}

		std::string partMsg = ":" + client.getPrefix() + " PART " + channelName;
		if (!reason.empty())
			partMsg += " :" + reason;
		partMsg += "\r\n";

		chan.broadcast(partMsg);

		chan.removeMember(client.getFd());
		
		if (chan.getMemberCount() == 0)
		{
			_channel.erase(channelName);
		}
	}
}