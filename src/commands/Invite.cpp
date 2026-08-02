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
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.size() < 2)
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }

    Channel *channel = server.getChannel(msg.params[1]);
    if (channel == NULL)
    {
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return;
    }
    if (!channel->isAMemberInChannel(client.getNickName()))
    {
        client.sendMessage(":ircserv 442 * :You're not on that channel\r\n");
        return;
    }
    if (!channel->isOperator(client.getNickName()))
    {
        client.sendMessage(":ircserv 482 * :You're not channel operator\r\n");
        return;
    }

    if (!handlTarget(channel, msg, client, server))
    {
        client.sendMessage(":ircserv 401 * :No such nick/channel\r\n");
    }
}