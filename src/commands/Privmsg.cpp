#include "ircserver.hpp"


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
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return;
    }
    if (!channel->isClientInChannel(&client))
    {
        client.sendMessage(":ircserv 442 * :You're not on that channel\r\n");
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
    client.sendMessage(":ircserv 401 * :No such nick/channel\r\n");
}

void handlePrivmsg(Client &client, IRCMessage &msg, Server &server)
{

    if (client.isRegistred() == false)
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return; 
    }
    if (msg.params.size() < 2)
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
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
