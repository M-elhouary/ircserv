#include "ircserver.hpp"


void handelTarget(Channel *channel, IRCMessage &msg, Client &client)
{
    if (channel->isAMemberInChannel(msg.params[1]))
    {
        std::string reson = " ";
        if(msg.params.size() > 2)
        {
            reson = msg.params[2];
        }
        std::string message = ":" + client.getNickName() + " KICK " + channel->getName() + " " + msg.params[1] + " :" + reson + "\r\n";
        channel->broadcast(message, &client);
        client.sendMessage(message);
        channel->removeClient(msg.params[1]);
        if (channel->isOperator(msg.params[1]))
        {
            channel->removeOperator(msg.params[1]);
        }
        return;
    }
    else
    {
        client.sendMessage(":ircserv 441 * :Target is not in channel\r\n");
        return;
    }
}

void handleKick(Client &client, IRCMessage &msg, Server &server)
{

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

    Channel *channel = server.getChannel(msg.params[0]);
    if (channel == NULL)
    {
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return;
    }
    else
    {

        if (!channel->isClientInChannel(&client))
        {
            client.sendMessage(":ircserv 442 * :You're not on that channel\r\n");
            return;
        }
        if (!channel->isOperator(client.getNickName()))
        {
            client.sendMessage(":ircserv 482 * :You're not channel operator\r\n");
            return;
        }
        handelTarget(channel, msg, client);
    }
}