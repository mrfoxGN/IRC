#include "../server/Server.hpp"

void Server::handleUser(Client &client, const std::string &line)
{
	if (!client.isPassOk())
	{
		client.sendMsg(":localhost 464 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Password incorrect or not provided\r\n");
		return;
	}

	if (client.isRegistered() || client.hasUser())
	{
		client.sendMsg(":localhost 462 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Unauthorized command (already registered)\r\n");
		return;
	}

	std::istringstream iss(line);
	std::string command;
	iss >> command; 

	std::string user, hostname, servername, realname;
	iss >> user >> hostname >> servername;

	std::getline(iss, realname);
	if (!realname.empty() && realname[0] == ' ')
		realname.erase(0, 1);
	if (!realname.empty() && realname[0] == ':')
		realname.erase(0, 1);

    if (user.empty() || hostname.empty() || servername.empty())
    {
        client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " USER :Not enough parameters\r\n");
        return;
    }
	user = "~" + user;
	
	if (realname.empty())
		realname = client.getNickname();

	client.setUsername(user);
	client.setRealname(realname);
	client.setHasUser(true);

	if (client.hasNick())
	{
		client.setRegistered(true);
		client.sendMsg(":localhost 001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!" + user + "@localhost\r\n");
	}
	else
	{
		// Wait for NICK command silently as per IRC protocol
	}
}
