#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include "../Client.hpp"
#include "../Server.hpp"


void handlePrivmsg(Client &client, IRCMessage &msg, Server &server);


#endif
