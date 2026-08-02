#include "ircserver.hpp"

bool ChannelValidationCheck(Client &client, IRCMessage &msg)
{

    if (!client.isRegistred())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return false;
    }

    if (msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return false;
    }
    if (msg.params[0].empty() || msg.params[0][0] != '#')
    {
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return false;
    }
    return true;
}

bool haslimitUser(Channel *channel)
{
    std::vector<Client *> ClientInChannel = channel->getMembers();
    int UserLimit = 0;
    for (std::vector<Client *>::iterator clientIT = ClientInChannel.begin(); clientIT != ClientInChannel.end(); clientIT++)
    {
        UserLimit++;
    }

    if (channel->getUserLimit() <= UserLimit && channel->getUserLimit() > 0)
        return true;
    return false;
}

bool PasswordCheck(Channel *channel, IRCMessage &msg)
{
    if (channel->getPassword() != "")
    {
        if (msg.params.size() < 2 || msg.params[1] != channel->getPassword())
        {
            return false;
        }
    }
    return true;
}

bool isClientInvited(Channel *channel, Client &client)
{
    if (channel->isInviteOnly())
    {
        if (!channel->isClientInvited(&client))
        {
            return false;
        }
    }
    return true;
}


void handleJoin(Client &client, IRCMessage &msg, Server &server)
{

    Channel *newChannel;

    if (!ChannelValidationCheck(client, msg))
        return;

    std::map<std::string, Channel *> &channels = server.getChannels();
    std::map<std::string, Channel *>::iterator it = channels.find(msg.params[0]);
    if (it == channels.end())
    {
        newChannel = new Channel(msg.params[0]);
        newChannel->addClient(&client);
        newChannel->addOperator(&client);
        channels[newChannel->getName()] = newChannel;

        client.sendMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostname() + " JOIN " + newChannel->getName() + "\r\n");

        // 353 RPL_NAMREPLY — first member is operator, prefix with @
        client.sendMessage(":ircserv 353 " + client.getNickName() + " = " + newChannel->getName() + " :@" + client.getNickName() + "\r\n");
       
        // :ircserv 331 alice #general :No topic is set
        client.sendMessage(":ircserv 331 " + client.getNickName() + " " + newChannel->getName() + " :No topic is set\r\n");

        // 366 RPL_ENDOFNAMES
        client.sendMessage(":ircserv 366 " + client.getNickName() + " " + newChannel->getName() + " :End of /NAMES list\r\n");
        return;
    }

    Channel *channel = it->second;

    // check if the client alreay in this channel
    if (channel->isClientInChannel(&client))
    {
        client.sendMessage(":ircserv 443 * " + channel->getName() + " :is already on channel\r\n");
        return;
    }

    if (haslimitUser(channel))
    {
        client.sendMessage(":ircserv 471 * :Cannot join channel, channel is full\r\n");
        return;
    }

    if (!isClientInvited(channel, client))
    {
        client.sendMessage(":ircserv 473 * :Cannot join, invite only\r\n");
        return;
    }

    if (!PasswordCheck(channel, msg))
    {
        client.sendMessage(":ircserv 475 * :Wrong channel key\r\n");
        return;
    }

    channel->addClient(&client);
    client.sendMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostname() + " JOIN " + channel->getName() + "\r\n");
    // broadcast the join message to all clients in the channel except the joining client   
    server.getChannel(channel->getName())->broadcast(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostname() + " JOIN " + channel->getName() + "\r\n", &client);
    // 332 RPL_TOPIC — send topic if channel has one
     if (!channel->getTopic().empty())
        client.sendMessage(":ircserv 332 " + client.getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");

    // 353 RPL_NAMREPLY — build member list, @ prefix for operators
    std::string names = "";
    std::vector<Client *> members = channel->getMembers();
    for (size_t i = 0; i < members.size(); i++)
    {
        if (channel->isOperator(members[i]->getNickName()))
            names += "@";
        names += members[i]->getNickName();
        if (i < members.size() - 1)
            names += " ";

    }
    // send the member list to the joining client
    client.sendMessage(":ircserv 353 " + client.getNickName() + " = " + channel->getName() + " :" + names + "\r\n");

    // 366 RPL_ENDOFNAMES
    client.sendMessage(":ircserv 366 " + client.getNickName() + " " + channel->getName() + " :End of /NAMES list\r\n");
}
