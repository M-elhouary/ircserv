#include "ircserver.hpp"

// src/commands/Invite.cpp
// └── void handleInvite(Client& client, IRCMessage& msg, Server& server)
//     ├── check: isRegistred()              → 451
//     ├── check: params exist               → 461
//     ├── find channel                      → 403
//     ├── client in channel?                → 442
//     ├── client is operator?               → 482
//     ├── find target by nickname           → 401
//     ├── channel.addToInviteList(target)
//     └── send INVITE to target

bool handlTarget(Channel *channel, IRCMessage &msg, Client &client, Server &server)
{
    std::map<int, Client *> &Clients = server.getClients();
    for (std::map<int, Client *>::iterator it = Clients.begin(); it !=
                                                                 Clients.end();
         ++it)
    {
        if (it->second->getNickName() == msg.params[0])
        {
            channel->addToInvitListe(it->second);

            std::string message = ":" + client.getNickName() + " INVITE " + it->second->getNickName() + " " + channel->getName() + "\r\n";
            it->second->sendMessage(message);

            std::string messageToInveter = ":ircserv 341 " + client.getNickName() + " " + it->second->getNickName() + " " + channel->getName() + "\r\n";
            client.sendMessage(messageToInveter);

            return true;
        }
    }
    return false;
}

void handleInvite(Client &client, IRCMessage &msg, Server &server)
{
    // check valadition of client
    if (!client.isRegistred())
    {
        client.sendMessage("ircserv * 451 :  you not register ");
        return;
    }
    if (msg.params.size() < 2)
    {
        client.sendMessage("ircserv * 461 : number of argument not enough");
        return;
    }

    Channel *channel = server.getChannel(msg.params[1]);
    if (channel == NULL)
    {
        client.sendMessage("403 * : this channel doesn't exist\r\n");
        return;
    }
    if (!channel->isAMemberInChannel(client.getNickName()))
    {
        client.sendMessage("442 * : you are not on this channel\r\n");
        return;
    }
    if (!channel->isOperator(client.getNickName()))
    {
        client.sendMessage("ircserv * 482 : you are not a operator in" + channel->getName() + "channel");
        return;
    }

    if (!handlTarget(channel, msg, client, server))
    {
        client.sendMessage("401 * : no such nick/channel\r\n");
    }
}