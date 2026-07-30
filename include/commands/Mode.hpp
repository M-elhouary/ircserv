#ifndef MODE_HPP
#define MODE_HPP

#include "../Client.hpp"
#include "../Parser.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"


void handleMode(Client &client, IRCMessage &msg, Server &server);

#endif
