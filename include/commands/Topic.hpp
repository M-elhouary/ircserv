#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "../Client.hpp"
#include "../Parser.hpp"
#include "../Server.hpp"

void handleTopic(Client &client, IRCMessage &msg, Server &server);

#endif
