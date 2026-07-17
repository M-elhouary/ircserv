#include "ircserver.hpp"

void dispatch(Client &client, const std::string &line, Server &server)
{
    IRCMessage msg = Parser::parse(line);
    if(msg.command.empty())
        return;
    if(msg.command == "PASS")
        handlePass(client, msg, server);
    else if (msg.command == "NICK")
        nick::handleNick(client, msg, server);
    else if (msg.command == "USER")
        handleUser(client, msg, server);
    else if (msg.command == "JOIN")
        handleJoin(client, msg, server);
    else
        client.sendMessage(":ircserv 421 * :Unknown command\r\n");
}
