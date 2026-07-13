#ifndef USER_HPP
#define USER_HPP

#include "../Client.hpp"
#include "../Parser.hpp"
#include "../Server.hpp"

void handleUser(Client &client, IRCMessage &msg, Server &server);


#endif