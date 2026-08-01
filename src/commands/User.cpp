#include "ircserver.hpp"


void handleUser(Client &client, IRCMessage &msg, Server &server)
{

    (void) server;

    if(!client.getAutenticated())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return ;
    }

    if(msg.params.size() < 4 ||  msg.params[0].empty() || msg.params[1].empty() || msg.params[2].empty() || msg.params[3].empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }
    if(!client.getNickNameReceived())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return ;
    }
    if(client.isRegistred())
    {
        client.sendMessage(":ircserv 462 * :You may not reregister\r\n");
        return ;
    }
    client.setUserName(msg.params[0]);
    client.setRegistred(true);
    tryCompleteRegistration(client);
}
