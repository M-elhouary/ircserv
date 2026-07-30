#include "ircserver.hpp"

void handlePing(Client &client, const IRCMessage &msg, Server &) {
    if (msg.params.empty())
        client.sendMessage(":ircserv PONG :ircserv\r\n");
    else
        client.sendMessage(":ircserv PONG :" + msg.params[0] + "\r\n");
}
