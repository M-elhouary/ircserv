#ifndef PING_HPP
# define PING_HPP

# include "../Client.hpp"
# include "../Parser.hpp"
# include "../Server.hpp"

void handlePing(Client &client, const IRCMessage &msg, Server &);

#endif // PING_HPP
