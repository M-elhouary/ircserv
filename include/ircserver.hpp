#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <cerrno>
#include <poll.h>
#include <string>
#include <vector>
#include <cstdio>

#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Dispatcher.hpp"
#include "Parser.hpp"
#include "Reply.hpp"
#include "Server.hpp"

#include "commands/Invite.hpp"
#include "commands/Join.hpp"
#include "commands/Kick.hpp"
#include "commands/Mode.hpp"
#include "commands/Nick.hpp"
#include "commands/Pass.hpp"
#include "commands/Privmsg.hpp"
#include "commands/Topic.hpp"
#include "commands/User.hpp"
#include "commands/Registration.hpp"

#include "utils/Colors.hpp"
#include "utils/NumericReplies.hpp"
#include "utils/Utils.hpp"

#endif // IRCSERVER_HPP
