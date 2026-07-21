#include "../server/Server.hpp"

void invite(Client* client, Client* target, Channel* channel);

void Server::handleInvite(Client &client, const std::string &line)
{
    std::istringstream iss(line);
    std::string cmd;
    std::string targetNick;
    std::string channelName;

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

    iss >> cmd >> targetNick >> channelName;
    if (targetNick.empty() || channelName.empty())
    {
        client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::map<std::string, Channel>::iterator channelIt = _channel.find(channelName);
    if (channelIt == _channel.end())
    {
        client.sendMsg(":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

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

    invite(&client, target, &channelIt->second);
}

void invite(Client* client, Client* target, Channel* channel) {

    if (!channel->isMember(client->getFd())) {
        client->sendMsg(":localhost 442 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not on that channel\r\n");
        return;
    }

    if (channel->isInviteOnly() && !channel->isOperator(client->getFd())) {
        client->sendMsg(":localhost 482 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not channel operator\r\n");
        return;
    }

    if (channel->isMember(target->getFd())) {
        client->sendMsg(":localhost 443 " + client->getNickname() +
                        " " + target->getNickname() +
                        " " + channel->getName() +
                        " :is already on channel\r\n");
        return;
    }

    channel->addInvite(target->getFd());

    client->sendMsg(":localhost 341 " + client->getNickname() +
                    " " + target->getNickname() +
                    " " + channel->getName() + "\r\n");

    target->sendMsg(":" + client->getPrefix() +
                    " INVITE " + target->getNickname() +
                    " :" + channel->getName() + "\r\n");
}

