#include "../server/Server.hpp"

static std::string trimLeadingSpaces(const std::string &value)
{
    size_t index = value.find_first_not_of(" \t");
    if (index == std::string::npos)
        return "";
    return value.substr(index);
}

void topic(Client* client, Channel* channel,
           const std::string& newTopic, bool hasTopic)
{
    if (!channel->isMember(client->getFd()))
    {
        client->sendMsg(":localhost 442 " + client->getNickname()
         + " " + channel->getName() + " :You're not on that channel\r\n");
        return;
    }

    if (!hasTopic)
    {
        if (channel->getTopic().empty())
        {
            client->sendMsg(":localhost 331 " + client->getNickname() +
                            " " + channel->getName() +
                            " :No topic is set\r\n");
        }
        else
        {
            client->sendMsg(":localhost 332 " + client->getNickname() +
                            " " + channel->getName() +
                            " :" + channel->getTopic() + "\r\n");
        }
        return;
    }

    if (channel->isTopicRestricted() && !channel->isOperator(client->getFd()))
    {
        client->sendMsg(":localhost 482 " + client->getNickname() +
                        " " + channel->getName() +
                        " :You're not channel operator\r\n");
        return;
    }

    channel->setTopic(newTopic);
    channel->broadcast(":" + client->getPrefix() +
                       " TOPIC " + channel->getName() +
                       " :" + newTopic + "\r\n");
}

void Server::handleTopic(Client &client, const std::string &line)
{
    std::istringstream iss(line);
    std::string cmd;
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

    iss >> cmd >> channelName;
    if (channelName.empty())
    {
        client.sendMsg(":localhost 461 " + (client.getNickname().empty() ? "*" : client.getNickname()) + " TOPIC :Not enough parameters\r\n");
        return;
    }

    std::map<std::string, Channel>::iterator it = _channel.find(channelName);
    if (it == _channel.end())
    {
        client.sendMsg(":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
        return;
    }

    std::string rawTopic;
    std::getline(iss >> std::ws, rawTopic);
    rawTopic = trimLeadingSpaces(rawTopic);

    bool hasTopic = !rawTopic.empty();
    if (hasTopic && rawTopic[0] == ':')
        rawTopic.erase(0, 1);

    topic(&client, &it->second, rawTopic, hasTopic);
}