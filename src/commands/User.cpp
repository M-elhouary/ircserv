#include "ircserver.hpp"


void handleUser(Client &client, IRCMessage &msg, Server &server)
{

    (void) server;

    if(!client.getAutenticated())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return ;
    }

    tryCompleteRegistration(client);

    if(msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }

    client.setUserName(msg.params[0]);
    client.setRegistred(true);
    tryCompleteRegistration(client);


    if(client.isRegistred())
    {
        client.sendMessage(":ircserv 001 " + client.getNickName() + " :Welcome to the IRC server\r\n");
    }

}
