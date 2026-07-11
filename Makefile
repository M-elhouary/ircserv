NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

INCDIR = include
SRCDIR = src

SRCS = src/main.cpp \
       src/server/ServerInit.cpp \
       src/server/AcceptClient.cpp \
       src/server/Disconnect.cpp \
       src/server/PollLoop.cpp \
       src/server/Shutdown.cpp \
       src/commands/Invite.cpp \
       src/commands/Join.cpp \
       src/commands/Kick.cpp \
       src/commands/Mode.cpp \
       src/commands/Nick.cpp \
       src/commands/Pass.cpp \
       src/commands/Privmsg.cpp \
       src/commands/Topic.cpp \
       src/commands/User.cpp \
       src/models/Channel.cpp \
       src/models/Client.cpp \
       src/network/Buffer.cpp \
       src/network/PacketAssembler.cpp \
       src/network/Receiver.cpp \
       src/network/Sender.cpp \
       src/network/Socket.cpp \
       src/parser/Dispatcher.cpp \
       src/parser/Parser.cpp \
       src/replies/Reply.cpp \
       src/utils/NumericReplies.cpp \
       src/utils/Utils.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
