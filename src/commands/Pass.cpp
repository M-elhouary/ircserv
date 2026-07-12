// #include "ircserver.hpp"



void handlePass(Client &client, IRCMessage &msg, Server &server)
{
    if (client.getAutenticated())
    {
        client.sendMessage(":ircserv 462 * :You may not reregister\r\n");
        return ;
    }

    if(msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return ;
    }
    if(msg.params[0] == server.getPassword())
    {
        client.setAutenticated(true);
        return;
    }
    client.sendMessage(":ircserv 464 * :Password incorrect\r\n");
} 