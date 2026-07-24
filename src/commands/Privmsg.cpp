#include "ircserver.hpp"

// src/commands/Privmsg.cpp
// └── void handlePrivmsg(Client& client, IRCMessage& msg, Server& server)
//     ├── check: client.isRegistred()       → 451
//     ├── check: params exist               → 461
//     ├── target is channel (#)?
//     │   ├── find channel                  → 403
//     │   ├── client in channel?            → 404
//     │   └── channel.broadcast(message, client)
//     └── target is user?
//         ├── find client by nickname       → 401
//         └── target.sendMessage(message)


std::string BuildPrivmsgMessage(Client &client, std::string &target, std::string &message)
{
    std::string msg = ":" + client.getNickName() + " PRIVMSG " + target + " :" + message + "\r\n";
    return msg;
}



void MsgTochannel(Client &client, IRCMessage &msg, Server &server)
{
    Channel *channel = server.getChannel(msg.params[0]);
    if (channel == NULL)
    {
        client.sendMessage("403 * : this channel doesn't exist\r\n");
        return;
    }
    if (!channel->isClientInChannel(&client))
    {
        client.sendMessage("404 * the client not exist on this channel\r\n");
        return;
    }
    std::string message = BuildPrivmsgMessage(client, msg.params[0], msg.params[1]);
    channel->broadcast(message, &client);
}


void MsgToClient(Client &client, IRCMessage &msg, Server &server)
{
    std::map<int, Client *> &Clients = server.getClients();
    for (std::map<int, Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (it->second->getNickName() == msg.params[0])
        {
            std::string message = BuildPrivmsgMessage(client, msg.params[0], msg.params[1]);    
            it->second->sendMessage(message);
            return;
        }
    }
    client.sendMessage("401 * : this client doesn't exist\r\n");
}

void handlePrivmsg(Client &client, IRCMessage &msg, Server &server)
{

    if (client.isRegistred() == false)
    {
        client.sendMessage("451 :You have not registered\r\n");
        return;
    }
    if (msg.params.size() < 2)
    {
        client.sendMessage("461 PRIVMSG :Not enough parameters\r\n");
        return;
    }
    if(msg.params[0][0] == '#')
    {
        MsgTochannel(client, msg, server);
    }
    else
    {
        MsgToClient(client, msg, server);   
    }


}
