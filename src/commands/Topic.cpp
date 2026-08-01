#include "ircserver.hpp"

// src/commands/Topic.cpp
// └── void handleTopic(Client& client, IRCMessage& msg, Server& server)
//     ├── check: isRegistred()              → 451
//     ├── find channel                      → 403
//     ├── client in channel?                → 442
//     ├── no params? → send current topic (332)
//     ├── topicRestricted + not operator?   → 482
//     ├── channel.setTopic(params[1])
//     └── broadcast new topic

void handleTopic(Client &client, IRCMessage &msg, Server &server)
{

    // validation of client
    if (!client.isRegistred())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }
    // channel validation
    Channel *channel = server.getChannel(msg.params[0]);
    if (channel == NULL)
    {
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return;
    }
    if (!channel->isClientInChannel(&client))
    {
        client.sendMessage(":ircserv 442 * :You're not on that channel\r\n");
        return;
    }
    // if no topic is provided, send the current topic
    if (msg.params.size() == 1)
    {
        if (channel->getTopic().empty())
            client.sendMessage(":ircserv 331 " + client.getNickName() + " " + channel->getName() + " :No topic is set\r\n");
        else
            client.sendMessage(":ircserv 332 " + client.getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");
        return;
    }
    // if the channel is topic restricted and the client is not an operator, send an error
    if ((channel->isTopicRestricted() && !channel->isOperator(client.getNickName())))
    {
        client.sendMessage(":ircserv 482 * :You're not channel operator\r\n");
        return;
    }
    // set the new topic and broadcast it to all clients in the channel
    channel->setTopic(msg.params[1]);
    client.sendMessage(":ircserv 332 " + client.getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");   
    std::string message = ":" + client.getNickName() + " TOPIC " + channel->getName() + " :" + msg.params[1] + "\r\n";
    channel->broadcast(message, &client);
}