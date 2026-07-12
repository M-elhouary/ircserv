#ifndef NICK_HPP
#define NICK_HPP

#include "../Client.hpp"
#include "../Parser.hpp"
#include "../Server.hpp"

class nick
{
    public:
        void handleNick(Client &client, IRCMessage &msg, Server &server);
        bool isValidNickName(std::string nickanme);
        bool isAlreadytaken(std::string nickname, Server &server);
};

#endif