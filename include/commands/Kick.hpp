#ifndef KICK_HPP
#define KICK_HPP

#include "../Client.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"

void handleKick(Client &client, IRCMessage &msg, Server &server);

#endif
