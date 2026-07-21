#include "../server/Server.hpp"
#include "../Channel/Channel.hpp"
#include "../Client/Client.hpp"

#include <sstream>
#include <map>
#include <string>

void Server::handlePart(Client &client, const std::string &line)
{
	if (!client.isRegistered())
	{
		client.sendMsg(
			":localhost 451 " +
			(client.getNickname().empty() ? "*" : client.getNickname()) +
			" :You have not registered\r\n"
		);
		return;
	}

	std::istringstream iss(line);
	std::string command;

	iss >> command;

	std::string channelsStr;

	if (!(iss >> channelsStr))
	{
		client.sendMsg(
			":localhost 461 " +
			client.getNickname() +
			" PART :Not enough parameters\r\n"
		);
		return;
	}

	std::string reason;
	std::string firstReasonPart;

	if (iss >> firstReasonPart)
	{
		if (!firstReasonPart.empty() && firstReasonPart[0] == ':')
			reason = firstReasonPart.substr(1);
		else
			reason = firstReasonPart;

		std::string rest;
		std::getline(iss, rest);
		reason += rest;
	}

	std::istringstream channelStream(channelsStr);
	std::string channelName;

	while (std::getline(channelStream, channelName, ','))
	{
		std::string::size_type start =
			channelName.find_first_not_of(" \t");

		std::string::size_type end =
			channelName.find_last_not_of(" \t");

		if (start == std::string::npos)
			continue;

		channelName =
			channelName.substr(start, end - start + 1);

		std::map<std::string, Channel>::iterator channelIt =
			_channel.find(channelName);

		if (channelIt == _channel.end())
		{
			client.sendMsg(
				":localhost 403 " +
				client.getNickname() + " " +
				channelName +
				" :No such channel\r\n"
			);
			continue;
		}

		Channel &channel = channelIt->second;

		if (!channel.isMember(client.getFd()))
		{
			client.sendMsg(
				":localhost 442 " +
				client.getNickname() + " " +
				channelName +
				" :You're not on that channel\r\n"
			);
			continue;
		}

		// Save operator state before removing the member.
		bool wasOperator =
			channel.isOperator(client.getFd());

		std::string partMessage =
			":" + client.getPrefix() +
			" PART " + channelName;

		if (!reason.empty())
			partMessage += " :" + reason;

		partMessage += "\r\n";

		// Broadcast before removing the client,
		// so the leaving client also receives the PART message.
		channel.broadcast(partMessage);

		// removeMember() must remove the fd from both
		// _members and _operators.
		channel.removeMember(client.getFd());

		// Delete the channel if no members remain.
		if (channel.getMemberCount() == 0)
		{
			_channel.erase(channelIt);
			continue;
		}

		// If the departing user was the last operator,
		// promote the first remaining member.
		if (wasOperator && channel.getOperators().empty())
		{
			const std::map<int, Client *> &members =
				channel.getMembers();

			if (!members.empty())
			{
				Client *newOperator =
					members.begin()->second;

				if (newOperator != NULL)
				{
					channel.addOperator(newOperator);

					std::string modeMessage =
						":localhost MODE " +
						channelName +
						" +o " +
						newOperator->getNickname() +
						"\r\n";

					channel.broadcast(modeMessage);
				}
			}
		}
	}
}