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
    else if (msg.command == "PRIVMSG")
        handlePrivmsg(client, msg, server);
    else if (msg.command == "KICK")
        handleKick(client, msg, server);
    else if (msg.command == "INVITE")
        handleInvite(client, msg, server);
    else if (msg.command == "TOPIC")
        handleTopic(client, msg, server);
    else if (msg.command == "MODE")
        handleMode(client, msg, server);
    else if (msg.command == "PING")
        handlePing(client, msg, server);
    else
        client.sendMessage(":ircserv 421 * :Unknown command\r\n");
}
