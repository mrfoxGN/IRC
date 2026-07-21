#include "../server/Server.hpp"
#include <sstream>

static std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::istringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

void Server::handleJoin(Client &client, const std::string &line)
{
	std::istringstream iss(line);
    std::string cmd;

    iss >> cmd;

    if (!client.isPassOk())
    {
        client.sendMsg(":localhost 464 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :Password incorrect\r\n");
        return;
    }

    if (!client.isRegistered())
    {
        client.sendMsg(":localhost 451 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " :You have not registered\r\n");
        return;
    }

    std::string channels_str;
    std::string keys_str;

    iss >> channels_str;
    iss >> keys_str;

    if (channels_str.empty())
    {
        client.sendMsg(":localhost 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::vector<std::string> channels = split(channels_str, ',');
    std::vector<std::string> keys = split(keys_str, ',');

    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string channel = channels[i];

        std::string key = "";
        if (i < keys.size())
            key = keys[i];

        joinChannel(client, channel, key);
    }
}
void Server::joinChannel(Client &client, const std::string &channel, const std::string &key)
{
    if (channel.empty() || (channel[0] != '#' && channel[0] != '&'))
    {
        client.sendMsg(":localhost 403 " + client.getNickname() + " " + channel + " :No such channel\r\n");
        return;
    }

    if (_channel.find(channel) == _channel.end())
        _channel.insert(std::make_pair(channel, Channel(channel)));

    Channel &ch = _channel.find(channel)->second;

    if (ch.isMember(client.getFd())){
        std::cout << "Client " << client.getNickname() << " already on channel " << channel << std::endl;
        return;
    }
    if (ch.hasPassword() && ch.getPassword() != key)
    {
        client.sendMsg(":localhost 475 " + client.getNickname() + " " + channel + " :Cannot join channel (bad key)\r\n");
        return;
    }
    if (ch.hasUserLimit() && ch.getMemberCount() >= ch.getUserLimit())
    {
        client.sendMsg(":localhost 471 " + client.getNickname() + " " + channel + " :Cannot join channel (+l)\r\n");
        return;
    }

    if (ch.isInviteOnly() && !ch.isInvited(client.getFd()))
    {
        client.sendMsg(":localhost 473 " + client.getNickname() + " " + channel + " :Cannot join channel (+i)\r\n");
        return;
    }

    if (ch.isInvited(client.getFd()))
        ch.removeInvite(client.getFd());
    
    bool isFirst = (ch.getMemberCount() == 0);

    ch.addMember(&client);

    if (isFirst)
        ch.addOperator(&client);

    std::string msg = ":" + client.getNickname() + "!" +
                      client.getUsername() + "@localhost JOIN :" +
                      channel + "\r\n";

    ch.broadcast(msg);

    if (ch.getTopic().empty())
        client.sendMsg(":localhost 331 " + client.getNickname() + " " + channel + " :No topic is set\r\n");
    else
        client.sendMsg(":localhost 332 " + client.getNickname() + " " + channel + " :" + ch.getTopic() + "\r\n");

    std::string namesList;
    std::map<int, Client*>::const_iterator it;
    const std::map<int, Client*>& members = ch.getMembers();
    for (it = members.begin(); it != members.end(); ++it)
    {
        if (!namesList.empty())
            namesList += " ";
        if (ch.isOperator(it->first))
            namesList += "@";
        namesList += it->second->getNickname();
    }

    client.sendMsg(":localhost 353 " + client.getNickname() + " = " + channel + " :" + namesList + "\r\n");
    client.sendMsg(":localhost 366 " + client.getNickname() + " " + channel + " :End of /NAMES list\r\n");
}