#include "ircserver.hpp"

// src/commands/Kick.cpp
// └── void handleKick(Client& client, IRCMessage& msg, Server& server)
//     ├── check: isRegistred()              → 451
//     ├── check: params exist               → 461
//     ├── find channel                      → 403
//     ├── client in channel?                → 442
//     ├── client is operator?               → 482
//     ├── target in channel?                → 441
//     ├── channel.removeClient(target)
//     └── broadcast KICK message

void handelTarget(Channel *channel, IRCMessage &msg, Client &client)
{
    if (channel->isAMemberInChannel(msg.params[1]))
    {
        channel->removeClient(msg.params[1]);
        if (channel->isOperator(msg.params[1]))
        {
            channel->removeOperator(msg.params[1]);
        }
        std::string reson = " ";
        if(msg.params.size() > 2)
        {
            reson = msg.params[2];
        }
        std::string message = ":" + client.getNickName() + " KICK " + channel->getName() + " " + msg.params[1] + " :" + reson + "\r\n";
        channel->broadcast(message, &client);
        return;
    }
    else
    {
        client.sendMessage("ircserv * 441 : this client doesn't exist on this channel\r\n");
        return;
    }
}

void handleKick(Client &client, IRCMessage &msg, Server &server)
{

    // check valadition of client
    if (!client.isRegistred())
    {
        client.sendMessage("ircserv * 452 :  you not register ");
        return;
    }
    // param for cmd
    if (msg.params.size() < 2)
    {
        client.sendMessage("ircserv * 461 : number of argument not enough");
        return;
    }

    Channel *channel = server.getChannel(msg.params[0]);
    if (channel == NULL)
    {
        client.sendMessage("403 * : this channel doesn't exist\r\n");
        return;
    }
    else
    {

        if (!channel->isClientInChannel(&client))
        {
            client.sendMessage("404 * the client not exist on this channel\r\n");
            return;
        }
        if (!channel->isOperator(client.getNickName()))
        {
            client.sendMessage("ircserv * 482 : you are not a operator in" + channel->getName() + "channel");
            return;
        }
        // target
        handelTarget(channel, msg, client);
    }
}