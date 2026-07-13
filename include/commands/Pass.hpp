#ifndef PASS_HPP
# define PASS_HPP

# include "../Client.hpp"
# include "../Parser.hpp"
# include "../Server.hpp"

void handlePass(Client &client, IRCMessage &msg, Server &server);

#endif // PASS_HPP