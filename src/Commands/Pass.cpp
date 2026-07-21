#include "../server/Server.hpp"

void Server::handlePass(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	std::string pass;
	iss >> pass;

	if (pass.empty())
	{
		client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " PASS :Not enough parameters\r\n");
	}
	else if (client.isRegistered() || client.isPassOk())
	{
		client.sendMsg(":localhost 462 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Unauthorized command (already registered)\r\n");
	}
	else if (pass != _password)
	{
		client.sendMsg(":localhost 464 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Password incorrect\r\n");
	}
	else
	{
		client.setPassOk(true);
		cout << "Client FD " << client.getFd() << " provided correct password." << endl;
	}
}
