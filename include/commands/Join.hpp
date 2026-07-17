#ifndef JOIN_HPP
# define JOIN_HPP



# include "../Client.hpp"
# include "../Parser.hpp" 
# include "../Server.hpp"
# include "../Channel.hpp"


void handleJoin(Client &client, IRCMessage &msg, Server &server);

#endif