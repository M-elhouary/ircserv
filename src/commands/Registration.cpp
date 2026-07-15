
#include "ircserver.hpp"

void tryCompleteRegistration(Client &client)
{
    if (client.isRegistred() && !client.getWelcomeSent())
    {
        client.sendMessage(":ircserv 001 " + client.getNickName() + " :Welcome to the IRC server\r\n");
        client.setWelcomeSent(true);
    }
}