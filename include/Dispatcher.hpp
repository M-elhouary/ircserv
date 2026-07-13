#ifndef DISPATCHER_HPP

#define DISPATCHER_HPP



#include "Client.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "commands/Pass.hpp"
#include "commands/Nick.hpp"
#include "commands/User.hpp"

void dispatch(Client &client, const std::string &line, Server &server);


#endif