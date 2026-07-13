#ifndef NICK_HPP
#define NICK_HPP

#include "../Client.hpp"
#include "../Parser.hpp"
#include "../Server.hpp"

class nick
{
    public:
        void static  handleNick(Client &client, IRCMessage &msg, Server &server);
        bool static  isValidNickName(std::string nickanme);
        bool static isAlreadytaken(std::string nickname, Server &server);
};

#endif