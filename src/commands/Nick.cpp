#include "ircserver.hpp"


bool nick::isValidNickName(std::string nickname)
{
    if(nickname.find(' ') != std::string::npos || nickname.empty())
        return false;
    if(!isalpha(nickname[0]) || (ispunct(nickname[0]) && (nickname[0] != '-' || nickname[0] != '_') ))
        return false;
    return true;
}


bool nick::isAlreadytaken(std::string nickname, Server &server)
{

    std::map<int, Client *> &clients = server.getClients();
    
    for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it).second->getNickName() == nickname)
            return true;
    }
    return false;
}



void nick::handleNick(Client &client, IRCMessage &msg, Server &server)
{
    if(msg.params.empty())
    {
        client.sendMessage(":ircserver 431 * : No nickname given\r\n");
        return;
    }

    if(!isValidNickName(msg.params[0]))
    {
        client.sendMessage(":ircserver 432 * : Erroneous nickname\r\n");
        return;
    }
    if(isAlreadytaken(msg.params[0], server))
    {
        client.sendMessage(":ircserver 433 * :Nickname is already in use\r\n");
        return;
    }
}