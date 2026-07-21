#include "../server/Server.hpp"
#include <sstream>

void Server::handleMan(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string cmd;
	std::string targetCmd;
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	iss >> cmd >> targetCmd;

	if (targetCmd.empty())
	{
		client.sendMsg(":localhost NOTICE " + nick + " :Available commands: JOIN, PART, KICK, PRIVMSG, NICK, USER, PASS, TOPIC, MODE, INVITE\r\n");
		client.sendMsg(":localhost NOTICE " + nick + " :Use MAN <command> to see how to use a specific command.\r\n");
		return;
	}

	for (size_t i = 0; i < targetCmd.length(); ++i)
		targetCmd[i] = toupper(targetCmd[i]);

	if (targetCmd == "JOIN")
		client.sendMsg(":localhost NOTICE " + nick + " :JOIN <channel>{,<channel>} [<key>{,<key>}] - Joins a channel. Example: JOIN #general\r\n");
	else if (targetCmd == "PART")
		client.sendMsg(":localhost NOTICE " + nick + " :PART <channel>{,<channel>} [<reason>] - Leaves a channel. Example: PART #general :Gotta go\r\n");
	else if (targetCmd == "KICK")
		client.sendMsg(":localhost NOTICE " + nick + " :KICK <channel> <user> [<reason>] - Kicks a user from a channel. Example: KICK #general Bob :Spamming\r\n");
	else if (targetCmd == "PRIVMSG")
		client.sendMsg(":localhost NOTICE " + nick + " :PRIVMSG <receiver>{,<receiver>} <text to be sent> - Sends a message. Example: PRIVMSG Bob :Hello!\r\n");
	else if (targetCmd == "NICK")
		client.sendMsg(":localhost NOTICE " + nick + " :NICK <nickname> - Changes your nickname. Example: NICK NewName\r\n");
	else if (targetCmd == "USER")
		client.sendMsg(":localhost NOTICE " + nick + " :USER <username> <hostname> <servername> <realname> - Sets your user details. Example: USER bob 0 * :Bob Smith\r\n");
	else if (targetCmd == "PASS")
		client.sendMsg(":localhost NOTICE " + nick + " :PASS <password> - Sets the connection password. Example: PASS secret123\r\n");
	else if (targetCmd == "TOPIC")
		client.sendMsg(":localhost NOTICE " + nick + " :TOPIC <channel> [<topic>] - Changes or views the topic of a channel. Example: TOPIC #general :New Topic\r\n");
	else if (targetCmd == "MODE")
	{
		client.sendMsg(":localhost NOTICE " + nick + " :MODE <channel> <flags> [<args>] - Changes channel modes. \r\n");
		client.sendMsg(":localhost NOTICE " + nick + " :Flags: +i (invite-only), +t (topic restricted), +k (password), +o (operator), +l (user limit)\r\n");
		client.sendMsg(":localhost NOTICE " + nick + " :Usage: MODE #channel +i (sets invite-only), MODE #channel -k (removes password)\r\n");
	}
	else if (targetCmd == "INVITE")
		client.sendMsg(":localhost NOTICE " + nick + " :INVITE <nickname> <channel> - Invites a user to a channel. Example: INVITE Bob #general\r\n");
	else
		client.sendMsg(":localhost NOTICE " + nick + " :No manual entry for " + targetCmd + "\r\n");
}