#include "../server/Server.hpp"

void kick(Client* client, Client* target,
          Channel* channel, const std::string& reason);

void Server::handleKick(Client &client, const std::string &line)
{
    std::istringstream iss(line);
    std::string cmd;
    std::string channelName;
    std::string targetNick;

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

    iss >> cmd >> channelName >> targetNick;
    if (channelName.empty() || targetNick.empty())
    {
        client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " KICK :Not enough parameters\r\n");
        return;
    }

    std::map<std::string, Channel>::iterator channelIt = _channel.find(channelName);
    if (channelIt == _channel.end())
    {
        client.sendMsg(":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    std::string reason;
    std::getline(iss >> std::ws, reason);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    if (reason.empty())
        reason = "Kicked";

    Client *target = NULL;
    std::map<int, Client>::iterator clientIt;
    for (clientIt = _clients.begin(); clientIt != _clients.end(); ++clientIt)
    {
        if (clientIt->second.getNickname() == targetNick)
        {
            target = &clientIt->second;
            break;
        }
    }

    if (!target)
    {
        client.sendMsg(":localhost 401 " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n");
        return;
    }

    kick(&client, target, &channelIt->second, reason);
}

void kick(Client* client, Client* target,
          Channel* channel, const std::string& reason) {

    if (!channel->isMember(client->getFd())) {
        client->sendMsg(":localhost 442 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not on that channel\r\n");
        return;
    }

    if (!channel->isOperator(client->getFd())) {
        client->sendMsg(":localhost 482 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not channel operator\r\n");
        return;
    }

    if (!channel->isMember(target->getFd())) {
        client->sendMsg(":localhost 441 " + client->getNickname() +
                        " " + target->getNickname() +
                        " " + channel->getName() +
                        " :They aren't on that channel\r\n");
        return;
    }

    channel->broadcast(":" + client->getPrefix() +
                       " KICK " + channel->getName() +
                       " " + target->getNickname() +
                       " :" + reason + "\r\n");

    channel->removeMember(target->getFd());
}