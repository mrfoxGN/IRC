#include <cstdlib>
#include "../server/Server.hpp"

void mode(Client* client, Channel* channel,
          const std::string& modeStr,
          const std::vector<std::string>& params,
          const std::map<int, Client*>& allClients);

static std::string buildModeString(Channel *channel)
{
    std::string modeStr = "+";
    if (channel->isInviteOnly())
        modeStr += "i";
    if (channel->isTopicRestricted())
        modeStr += "t";
    if (channel->hasPassword())
        modeStr += "k";
    if (channel->hasUserLimit())
        modeStr += "l";
    return modeStr;
}

void Server::handleMode(Client &client, const std::string &line)
{
    std::istringstream iss(line);
    std::string cmd;
    std::string channelName;
    std::string modeStr;

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

    iss >> cmd >> channelName;
    if (channelName.empty())
    {
        client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " MODE :Not enough parameters\r\n");
        return;
    }

    std::map<std::string, Channel>::iterator it = _channel.find(channelName);
    if (it == _channel.end())
    {
        client.sendMsg(":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!(iss >> modeStr))
    {
        std::string modeReply = buildModeString(&it->second);
        std::ostringstream reply;
        reply << ":localhost 324 " << (client.getNickname().empty() ? "*" : client.getNickname())
              << " " << channelName << " " << modeReply;
        if (it->second.hasUserLimit())
            reply << " " << it->second.getUserLimit();
        reply << "\r\n";
        client.sendMsg(reply.str());
        return;
    }

    std::vector<std::string> params;
    std::string param;
    while (iss >> param)
        params.push_back(param);

    std::map<int, Client*> allClients;
    std::map<int, Client>::iterator clientIt;
    for (clientIt = _clients.begin(); clientIt != _clients.end(); ++clientIt)
        allClients[clientIt->first] = &clientIt->second;

    mode(&client, &it->second, modeStr, params, allClients);
}

void mode(Client* client, Channel* channel,
          const std::string& modeStr,
          const std::vector<std::string>& params,
          const std::map<int, Client*>& allClients) {

    if (!channel->isOperator(client->getFd())) {
        client->sendMsg(":localhost 482 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not channel operator\r\n");
        return;
    }

    if (modeStr.empty()) return;

    bool adding = true;
    size_t paramIndex = 0;

    for (size_t i = 0; i < modeStr.size(); i++) {
        char c = modeStr[i];

        if (c == '+') { adding = true;  continue; }
        if (c == '-') { adding = false; continue; }

        if (c == 'i') {
            channel->setInviteOnly(adding);
            channel->broadcast(":" + client->getPrefix() +
                               " MODE " + channel->getName() +
                               (adding ? " +i" : " -i") + "\r\n");
        }
        else if (c == 't') {
            channel->setTopicRestricted(adding);
            channel->broadcast(":" + client->getPrefix() +
                               " MODE " + channel->getName() +
                               (adding ? " +t" : " -t") + "\r\n");
        }
        else if (c == 'k') {
            if (adding) {
                if (paramIndex >= params.size()) {
                    client->sendMsg(":localhost 461 " + client->getNickname() +
                                    " MODE :Not enough parameters\r\n");
                    return;
                }
                channel->setPassword(params[paramIndex++]);
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " +k " + channel->getPassword() + "\r\n");
            } else {
                channel->removePassword();
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " -k\r\n");
            }
        }
        else if (c == 'l') {
            if (adding) {
                if (paramIndex >= params.size()) {
                    client->sendMsg(":localhost 461 " + client->getNickname() +
                                    " MODE :Not enough parameters\r\n");
                    return;
                }
                int limit = std::atoi(params[paramIndex].c_str());
                channel->setUserLimit(limit);
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " +l " + params[paramIndex] + "\r\n");
                paramIndex++;
            } else {
                channel->removeUserLimit();
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " -l\r\n");
            }
        }
        else if (c == 'o') {
            if (paramIndex >= params.size()) {
                client->sendMsg(":localhost 461 " + client->getNickname() +
                                " MODE :Not enough parameters\r\n");
                return;
            }
            std::string targetNick = params[paramIndex++];

            Client* target = NULL;
            std::map<int, Client*>::const_iterator it;
            for (it = allClients.begin(); it != allClients.end(); ++it) {
                if (it->second->getNickname() == targetNick) {
                    target = it->second;
                    break;
                }
            }

            if (!target || !channel->isMember(target->getFd())) {
                client->sendMsg(":localhost 441 " + client->getNickname() +
                                " " + targetNick +
                                " " + channel->getName() +
                                " :They aren't on that channel\r\n");
                return;
            }

            if (adding) {
                channel->addOperator(target);
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " +o " + targetNick + "\r\n");
            } else {
                channel->removeOperator(target->getFd());
                channel->broadcast(":" + client->getPrefix() +
                                   " MODE " + channel->getName() +
                                   " -o " + targetNick + "\r\n");
            }
        }
    }
}