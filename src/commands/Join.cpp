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
        client.sendMessage("ircserv : the Channel name shoulde be starte by '#' ");
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

        client.sendMessage(":ircserv  JOIN " + newChannel->getName() + "\r\n");
        return;
    }

    Channel *channel = it->second;

    // check if the client alreay in this channel
    if (channel->isClientInChannel(&client))
    {
        client.sendMessage(":ircserv 443 * " + channel->getName() + " :is already on channel\r\n");
        return;
    }

    if(haslimitUser(channel))
    {
        client.sendMessage(":ircserv 471 *: cannot join channel has limit user");
        return;
    }

    if(!isClientInvited(channel, client))
    {
        client.sendMessage(":ircserv 473 *: cannot join channel has invite only");
        return;
    }

    if(!PasswordCheck(channel, msg))
    {
        client.sendMessage("ircserv 475 * : the password is incorrect");
        return;
    }

    channel->addClient(&client);
    client.sendMessage(":ircserv  JOIN " + channel->getName() + "\r\n");
}