#include "ircserver.hpp"

void dispatch(Client &client, const std::string &line, Server &server)
{
    IRCMessage msg = Parser::parse(line);
    if(msg.command.empty())
        return;
    if(msg.command == "PASS") // Handle the PASS command
        handlePass(client, msg, server);
    else if (msg.command == "NICK") // Handle the NICK command
        nick::handleNick(client, msg, server);
    else if (msg.command == "USER") // Handle the USER command
        handleUser(client, msg, server);
    else if (msg.command == "JOIN") // Handle the JOIN command
        handleJoin(client, msg, server);
    else if (msg.command == "PRIVMSG") // Handle the PRIVMSG command
        handlePrivmsg(client, msg, server);
    else if (msg.command == "KICK") // Handle the KICK command
        handleKick(client, msg, server);
    else if (msg.command == "INVITE") // Handle the INVITE command
        handleInvite(client, msg, server);
    else if (msg.command == "TOPIC") // Handle the TOPIC command
        handleTopic(client, msg, server);
    else if (msg.command == "MODE") // Handle the MODE command
        handleMode(client, msg, server);
    else if (msg.command == "PING") // Handle the PING command
        handlePing(client, msg, server);
    else
        client.sendMessage(":ircserv 421 * :Unknown command\r\n"); // Send an error message for unknown commands
}
    