#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <poll.h>
#include <string>
#include <vector>
#include <cstdio>
#include <csignal>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ostream>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>



#include "Channel.hpp"
#include "Client.hpp"
#include "Dispatcher.hpp"
#include "Parser.hpp"
#include "Server.hpp"

#include "commands/Invite.hpp"
#include "commands/Join.hpp"
#include "commands/Kick.hpp"
#include "commands/Mode.hpp"
#include "commands/Nick.hpp"
#include "commands/Pass.hpp"
#include "commands/Ping.hpp"
#include "commands/Privmsg.hpp"
#include "commands/Topic.hpp"
#include "commands/User.hpp"
#include "commands/Registration.hpp"

#endif // IRCSERVER_HPP
