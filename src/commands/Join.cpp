#include "ircserver.hpp"


void handleJoin(Client &client, IRCMessage &msg, Server &server)
{

    if(!client.isRegistred())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return ;
    }

    if(msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }
    std::map<std::string, Channel *> &channels = server.getChannels();
    for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
    {

        
        if(it->second->getName() == msg.params[0])
        {
            // check if the client alreay in this channel
            if(it->second->isClientInChannel(&client))
            {
                client.sendMessage(":ircserv 443 * " + it->second->getName() + " :is already on channel\r\n");
                return;
            }
            
            if(it->second->getUserLimit())
            {
                // mode +l 
                // limit of user in channel

            }

            if(it->second->isInveted())
            {

            }

            if(it->second->isInviteOnly())
            {

            }

            it->second->addClient(&client);
            client.sendMessage(":ircserv  JOIN " + it->second->getName() + "\r\n");
            break;
        }
        else
        {
            Channel *newChannel = new Channel(msg.params[0]);
            newChannel->addClient(&client);
            channels[newChannel->getName()] = newChannel;
            client.sendMessage(":ircserv  JOIN " + newChannel->getName() + "\r\n");
            break;
        }
    }


    


}