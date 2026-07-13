# ft_irc — Complete Project Map
> Developer 2 Reference Guide — IRC Protocol & Business Logic

---

## Table of Contents

1. [How IRC Server Works](#1-how-irc-server-works)
2. [Server Lifecycle](#2-server-lifecycle)
3. [Client Lifecycle](#3-client-lifecycle)
4. [Channel Lifecycle](#4-channel-lifecycle)
5. [Complete Data Structures](#5-complete-data-structures)
6. [Complete Function Map](#6-complete-function-map)
7. [IRC Message Format](#7-irc-message-format)
8. [Registration Flow](#8-registration-flow)
9. [JOIN Flow](#9-join-flow)
10. [PRIVMSG Flow](#10-privmsg-flow)
11. [Operator Commands](#11-operator-commands)
12. [MODE Command](#12-mode-command)
13. [Error Codes Reference](#13-error-codes-reference)
14. [What You Need From Developer 1](#14-what-you-need-from-developer-1)
15. [Phase Roadmap](#15-phase-roadmap)
16. [File Structure](#16-file-structure)

---

## 1. How IRC Server Works

```
┌─────────────────────────────────────────────────────────────┐
│                        IRC SERVER                            │
│                                                              │
│   ┌─────────────────┐         ┌───────────────────────────┐ │
│   │   NETWORKING    │         │      IRC LOGIC            │ │
│   │   (Dev 1)       │         │      (Dev 2 - You)        │ │
│   │                 │         │                           │ │
│   │  socket()       │         │  Parser                   │ │
│   │  bind()         │         │  Dispatcher               │ │
│   │  listen()       │         │  PASS / NICK / USER       │ │
│   │  poll()         │─────────▶  JOIN / PRIVMSG           │ │
│   │  accept()       │         │  KICK / INVITE            │ │
│   │  recv()         │         │  TOPIC / MODE             │ │
│   │  send()         │         │                           │ │
│   └─────────────────┘         └───────────────────────────┘ │
│                                                              │
│                    ┌──────────────┐                          │
│                    │    MODELS    │                          │
│                    │              │                          │
│                    │  Client      │                          │
│                    │  Channel     │                          │
│                    │  Server      │                          │
│                    └──────────────┘                          │
└─────────────────────────────────────────────────────────────┘
```

**Key principle:**
- Developer 1 owns everything that touches the **socket and bytes**
- You (Developer 2) own everything that gives **meaning to those bytes**
- The boundary between you is the `dispatch()` function

**→ Next:** [Server Lifecycle](#2-server-lifecycle)

---

## 2. Server Lifecycle

```
./ircserv 6667 mypassword
          │
          ▼
       main()
          │
          ▼
Server server(6667, "mypassword")
          │
          ├── socket()       → create listening socket (fd)
          ├── setsockopt()   → allow address reuse
          ├── bind()         → attach socket to port 6667
          ├── listen()       → mark socket as passive
          └── fcntl()        → set non-blocking mode
                │
                ▼
          ┌─────────────────────────────────┐
          │           POLL LOOP             │
          │                                 │
          │  poll(fds, nfds, -1)            │
          │  blocks until event occurs      │
          │                                 │
          │  event on server_fd             │
          │  → new client connecting        │
          │  → accept() → new Client object │
          │  → add to _clients map          │
          │  → add fd to poll fds           │
          │                                 │
          │  event on client_fd             │
          │  → recv() into client buffer    │
          │  → buffer has \r\n?             │
          │    YES → extract complete line  │
          │          dispatch(client, line) │
          │    NO  → wait for more data     │
          │                                 │
          │  recv() returns 0               │
          │  → client disconnected          │
          │  → cleanup client               │
          └─────────────────────────────────┘
```

**What the poll loop gives you:**
```
dispatch(client, "PASS mypassword\r\n", server)
dispatch(client, "NICK spider\r\n",     server)
dispatch(client, "USER spider 0 * :Real Name\r\n", server)
dispatch(client, "JOIN #general\r\n",   server)
```

**→ Next:** [Client Lifecycle](#3-client-lifecycle)

---

## 3. Client Lifecycle

```
CLIENT CONNECTS
      │
      ▼
┌─────────────────────────────────────────────────────┐
│  STATE: CONNECTING                                   │
│  _authenticated    = false                           │
│  _nicknameReceived = false                           │
│  _registered       = false                           │
└──────────────────────────┬──────────────────────────┘
                           │
                           │  client sends: PASS mypassword\r\n
                           ▼
                    handlePass()
                    ├── already authenticated? → 462 error
                    ├── no params?             → 461 error
                    ├── wrong password?        → 464 error + disconnect
                    └── correct!
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│  STATE: AUTHENTICATED                                │
│  _authenticated    = true                            │
│  _nicknameReceived = false                           │
│  _registered       = false                           │
└──────────────────────────┬──────────────────────────┘
                           │
                           │  client sends: NICK spider\r\n
                           ▼
                    handleNick()
                    ├── not authenticated?     → 451 error
                    ├── no params?             → 431 error
                    ├── invalid nickname?      → 432 error
                    ├── nickname taken?        → 433 error
                    └── valid!
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│  STATE: NICK RECEIVED                                │
│  _authenticated    = true                            │
│  _nicknameReceived = true                            │
│  _nickname         = "spider"                        │
│  _registered       = false                           │
└──────────────────────────┬──────────────────────────┘
                           │
                           │  client sends: USER spider 0 * :Real Name\r\n
                           ▼
                    handleUser()
                    ├── not authenticated?     → 451 error
                    ├── already registered?    → 462 error
                    ├── no params?             → 461 error
                    └── valid!
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│  STATE: REGISTERED                                   │
│  _authenticated    = true                            │
│  _nicknameReceived = true                            │
│  _registered       = true                            │
│  _nickname         = "spider"                        │
│  _username         = "spider"                        │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
              send: ":ircserv 001 spider :Welcome!\r\n"
                           │
                           ▼
              client can now: JOIN PRIVMSG KICK INVITE TOPIC MODE
                           │
                           ▼
              CLIENT DISCONNECTS
              → remove from all channels
              → notify channel members
              → remove from server._clients
              → close fd
              → delete Client object
```

**→ Next:** [Channel Lifecycle](#4-channel-lifecycle)

---

## 4. Channel Lifecycle

```
CLIENT SENDS: JOIN #general\r\n
                    │
                    ▼
            handleJoin()
                    │
                    ▼
         channel "#general" exists?
                    │
          ┌─────────┴─────────┐
          NO                  YES
          │                   │
          ▼                   ▼
    create Channel       check _inviteOnly
    _name = "#general"        │
          │              ┌────┴────┐
          │             YES        NO
          │              │         │
          │              ▼         ▼
          │        client in   check _userLimit
          │        _inviteList?     │
          │        ┌───┴───┐   ┌───┴───┐
          │       YES      NO  AT     NOT
          │        │       │  LIMIT   FULL
          │        │       ▼   │       │
          │        │     473   ▼       │
          │        │    error 471      │
          │        │          error    │
          └────────┴──────────────────┘
                           │
                           ▼
                  addClient(client)
                           │
                           ▼
                  first member?
                  → addOperator(client)
                           │
                           ▼
              send JOIN confirmation:
              ":spider!user@host JOIN #general\r\n"
              ":ircserv 353 spider = #general :@spider\r\n"
              ":ircserv 366 spider #general :End of NAMES\r\n"
                           │
                           ▼
              CLIENT SENDS: PRIVMSG #general :hello\r\n
                           │
                           ▼
                  handlePrivmsg()
                  → find channel
                  → loop _members
                  → send to each EXCEPT sender:
                  ":spider!user@host PRIVMSG #general :hello\r\n"
                           │
                           ▼
              CLIENT LEAVES (PART or disconnect)
                           │
                           ▼
                  removeClient(client)
                  broadcast: ":spider PART #general\r\n"
                           │
                           ▼
                  _members empty?
                  → delete Channel
                  → erase from server._channels
```

**→ Next:** [Complete Data Structures](#5-complete-data-structures)

---

## 5. Complete Data Structures

### Server
```cpp
class Server
{
    private:
        int                              _port;
        std::string                      _password;
        std::string                      _name;        // "ircserv"
        int                              _serverFd;    // listening socket
        std::vector<pollfd>              _fds;         // poll list
        std::map<int, Client*>           _clients;     // fd → client
        std::map<std::string, Channel*>  _channels;    // name → channel

    public:
        // Dev 1 implements:
        void                             start();
        void                             run();

        // Used by Dev 2:
        std::string                      getPassword() const;
        std::string                      getName() const;
        std::map<int, Client*>&          getClients();
        std::map<std::string, Channel*>& getChannels();
};
```

### Client
```cpp
class Client
{
    private:
        int          _fd;                // socket file descriptor
        std::string  _nickname;          // "spider"
        std::string  _username;          // "spider"
        std::string  _buffer;            // partial data from recv()
        bool         _authenticated;     // sent correct PASS
        bool         _nicknameReceived;  // sent valid NICK
        bool         _registered;        // sent USER, fully registered

    public:
        // constructor:
        Client(int fd);

        // getters:
        int         getFd()              const;
        std::string getNickName()        const;
        std::string getUserName()        const;
        bool        getAuthenticated()   const;
        bool        isRegistred()        const;

        // setters:
        void setNickName(const std::string& nickname);
        void setUserName(const std::string& username);
        void setAuthenticated(bool value);
        void setNickNameReceived(bool value);
        void setRegistred(bool value);

        // Dev 1 implements:
        void sendMessage(const std::string& message);
};
```

### Channel
```cpp
class Channel
{
    private:
        std::string           _channelName;      // "#general"
        std::string           _topic;            // channel subject
        std::string           _password;         // channel key
        int                   _userLimit;        // 0 = no limit
        bool                  _inviteOnly;       // +i mode
        bool                  _topicRestricted;  // +t mode
        std::vector<Client*>  _clients;          // all members
        std::vector<Client*>  _operators;        // channel ops
        std::vector<Client*>  _inviteList;       // invited users

    public:
        // constructor:
        Channel(const std::string& name);

        // member management:
        void addClient(Client* client);
        void removeClient(Client* client);
        bool isClientInChannel(Client* client) const;

        // operator management:
        void addOperator(Client* client);
        void removeOperator(Client* client);
        bool isOperator(Client* client) const;

        // invite management:
        void addToInviteList(Client* client);
        bool isInvited(Client* client) const;

        // getters:
        std::string           getName()            const;
        std::string           getTopic()           const;
        std::string           getPassword()        const;
        int                   getUserLimit()       const;
        bool                  isInviteOnly()       const;
        bool                  isTopicRestricted()  const;
        std::vector<Client*>& getMembers();
        std::vector<Client*>& getOperators();
        std::vector<Client*>& getInviteList();

        // setters:
        void setTopic(const std::string& topic);
        void setPassword(const std::string& password);
        void setUserLimit(int limit);
        void setInviteOnly(bool value);
        void setTopicRestricted(bool value);

        // broadcast (Dev 1 helps implement):
        void broadcast(const std::string& message, Client* exclude = NULL);
};
```

**→ Next:** [Complete Function Map](#6-complete-function-map)

---

## 6. Complete Function Map

```
YOUR FILES (Developer 2)
════════════════════════

src/parser/Parser.cpp
└── IRCMessage Parser::parse(std::string line)
    ├── stripCRLF(line)
    ├── extractPrefix(line, msg, pos)
    ├── extractCommand(line, msg, pos)
    └── extractParams(line, msg, pos)

src/parser/Dispatcher.cpp
└── void dispatch(Client& client, const std::string& line, Server& server)
    ├── Parser::parse(line) → IRCMessage
    └── routes to correct handler by msg.command

src/commands/Pass.cpp
└── void handlePass(Client& client, IRCMessage& msg, Server& server)
    ├── check: client.getAuthenticated()  → 462
    ├── check: msg.params.empty()         → 461
    ├── check: params[0] == password      → 464
    └── client.setAuthenticated(true)

src/commands/Nick.cpp
├── bool isValidNickname(const std::string& nickname)
├── bool isAlreadyTaken(const std::string& nickname, Server& server)
└── void handleNick(Client& client, IRCMessage& msg, Server& server)
    ├── check: client.getAuthenticated()  → 451
    ├── check: msg.params.empty()         → 431
    ├── check: isValidNickname()          → 432
    ├── check: isAlreadyTaken()           → 433
    ├── client.setNickName(params[0])
    └── client.setNickNameReceived(true)

src/commands/User.cpp
└── void handleUser(Client& client, IRCMessage& msg, Server& server)
    ├── check: client.getAuthenticated()  → 451
    ├── check: client.isRegistred()       → 462
    ├── check: msg.params.empty()         → 461
    ├── client.setUserName(params[0])
    ├── client.setRegistred(true)
    └── send 001 Welcome

src/commands/Join.cpp
└── void handleJoin(Client& client, IRCMessage& msg, Server& server)
    ├── check: client.isRegistred()       → 451
    ├── check: valid channel name (#)     → 403
    ├── find or create channel
    ├── check: inviteOnly + invited?      → 473
    ├── check: userLimit reached?         → 471
    ├── check: password matches?          → 475
    ├── channel.addClient(client)
    ├── first member? → channel.addOperator(client)
    └── send JOIN confirmation (JOIN + 353 + 366)

src/commands/Privmsg.cpp
└── void handlePrivmsg(Client& client, IRCMessage& msg, Server& server)
    ├── check: client.isRegistred()       → 451
    ├── check: params exist               → 461
    ├── target is channel (#)?
    │   ├── find channel                  → 403
    │   ├── client in channel?            → 404
    │   └── channel.broadcast(message, client)
    └── target is user?
        ├── find client by nickname       → 401
        └── target.sendMessage(message)

src/commands/Kick.cpp
└── void handleKick(Client& client, IRCMessage& msg, Server& server)
    ├── check: isRegistred()              → 451
    ├── check: params exist               → 461
    ├── find channel                      → 403
    ├── client in channel?                → 442
    ├── client is operator?               → 482
    ├── target in channel?                → 441
    ├── channel.removeClient(target)
    └── broadcast KICK message

src/commands/Invite.cpp
└── void handleInvite(Client& client, IRCMessage& msg, Server& server)
    ├── check: isRegistred()              → 451
    ├── check: params exist               → 461
    ├── find channel                      → 403
    ├── client in channel?                → 442
    ├── client is operator?               → 482
    ├── find target by nickname           → 401
    ├── channel.addToInviteList(target)
    └── send INVITE to target

src/commands/Topic.cpp
└── void handleTopic(Client& client, IRCMessage& msg, Server& server)
    ├── check: isRegistred()              → 451
    ├── find channel                      → 403
    ├── client in channel?                → 442
    ├── no params? → send current topic (332)
    ├── topicRestricted + not operator?   → 482
    ├── channel.setTopic(params[1])
    └── broadcast new topic

src/commands/Mode.cpp
└── void handleMode(Client& client, IRCMessage& msg, Server& server)
    ├── check: isRegistred()              → 451
    ├── find channel                      → 403
    ├── client is operator?               → 482
    ├── parse mode string (+i -i +t -t +k -k +o -o +l -l)
    └── apply each mode change

src/models/Channel.cpp
├── addClient() / removeClient() / isClientInChannel()
├── addOperator() / removeOperator() / isOperator()
├── addToInviteList() / isInvited()
├── broadcast()
└── all getters / setters


DEVELOPER 1 FILES
═════════════════

src/network/Socket.cpp      → socket/bind/listen setup
src/network/Buffer.cpp      → per-client recv() buffering
src/network/PacketAssembler → detect \r\n, build complete lines
src/network/Receiver.cpp    → read from fd into buffer
src/network/Sender.cpp      → write to fd (sendMessage impl)
src/server/Server.cpp       → main server object
src/server/ServerInit.cpp   → startup sequence
src/server/PollLoop.cpp     → poll() event loop
src/server/AcceptClient.cpp → accept() new connections
src/server/Disconnect.cpp   → client cleanup
src/server/Shutdown.cpp     → graceful shutdown
```

**→ Next:** [IRC Message Format](#7-irc-message-format)

---

## 7. IRC Message Format

Every IRC message follows this exact structure:

```
[':' PREFIX SPACE] COMMAND [PARAMS] '\r\n'
```

### Rules

```
1. PREFIX
   → optional
   → only present if line starts with ':'
   → everything between ':' and first space
   → who sent the message

2. COMMAND
   → always present
   → first word after prefix (or start of line)
   → can be word (NICK) or number (001)

3. PARAMETERS
   → separated by spaces
   → if parameter starts with ':' → rest of line is ONE parameter

4. \r\n
   → every message ends with carriage return + newline
   → your parser strips this first
```

### Examples

```
CLIENT → SERVER (no prefix):
"PASS mypassword\r\n"
"NICK spider\r\n"
"USER spider 0 * :Spider Real Name\r\n"
"JOIN #general\r\n"
"PRIVMSG #general :hello everyone\r\n"

SERVER → CLIENT (with prefix):
":ircserv 001 spider :Welcome to the server\r\n"
":spider!user@host JOIN #general\r\n"
":ircserv 353 spider = #general :@alice bob\r\n"
":ircserv 433 * spider :Nickname is already in use\r\n"
```

### Parsing Result (IRCMessage struct)

```
"USER spider 0 * :Spider Real Name\r\n"

IRCMessage {
    prefix  = ""
    command = "USER"
    params  = ["spider", "0", "*", "Spider Real Name"]
}

":ircserv 001 spider :Welcome\r\n"

IRCMessage {
    prefix  = "ircserv"
    command = "001"
    params  = ["spider", "Welcome"]
}
```

**→ Next:** [Registration Flow](#8-registration-flow)

---

## 8. Registration Flow

```
STEP 1: PASS
─────────────
Client:  PASS mypassword\r\n
Server checks:
  → already authenticated?    → :ircserv 462 * :You may not reregister\r\n
  → no params?                → :ircserv 461 * :Not enough parameters\r\n
  → wrong password?           → :ircserv 464 * :Password incorrect\r\n
  → correct!                  → _authenticated = true (no reply)

STEP 2: NICK
─────────────
Client:  NICK spider\r\n
Server checks:
  → not authenticated?        → :ircserv 451 * :You have not registered\r\n
  → no params?                → :ircserv 431 * :No nickname given\r\n
  → invalid nickname?         → :ircserv 432 * spider :Erroneous nickname\r\n
  → nickname taken?           → :ircserv 433 * spider :Nickname already in use\r\n
  → valid!                    → _nickname = "spider", _nicknameReceived = true

STEP 3: USER
─────────────
Client:  USER spider 0 * :Spider Real Name\r\n
Server checks:
  → not authenticated?        → :ircserv 451 * :You have not registered\r\n
  → already registered?       → :ircserv 462 * :You may not reregister\r\n
  → no params?                → :ircserv 461 * :Not enough parameters\r\n
  → valid!                    → _username = "spider", _registered = true
                              → :ircserv 001 spider :Welcome to the IRC server\r\n

REGISTRATION COMPLETE
──────────────────────
Client can now use: JOIN PRIVMSG KICK INVITE TOPIC MODE
```

**→ Next:** [JOIN Flow](#9-join-flow)

---

## 9. JOIN Flow

```
Client:  JOIN #general\r\n
         JOIN #general mypassword\r\n   (with password)

params[0] = "#general"
params[1] = "mypassword"  (optional)

Server checks:
  → not registered?           → :ircserv 451 * :You have not registered\r\n
  → invalid name (no #)?      → :ircserv 403 * #general :No such channel\r\n
  → channel full?             → :ircserv 471 * #general :Cannot join, full\r\n
  → invite only + not invited → :ircserv 473 * #general :Invite only channel\r\n
  → wrong password?           → :ircserv 475 * #general :Wrong channel key\r\n

SUCCESS - server sends 3 messages:

1. JOIN confirmation to ALL members including new client:
   ":spider!user@host JOIN #general\r\n"

2. Member list to new client:
   ":ircserv 353 spider = #general :@alice bob spider\r\n"
   (@ prefix means operator)

3. End of names list:
   ":ircserv 366 spider #general :End of /NAMES list\r\n"

4. If channel has topic:
   ":ircserv 332 spider #general :Welcome to general!\r\n"
```

**→ Next:** [PRIVMSG Flow](#10-privmsg-flow)

---

## 10. PRIVMSG Flow

```
TO A CHANNEL:
Client:  PRIVMSG #general :hello everyone\r\n

Server:
  → find channel #general          → 403 if not found
  → client in channel?             → 404 if not
  → broadcast to ALL members EXCEPT sender:
     ":spider!user@host PRIVMSG #general :hello everyone\r\n"

TO A USER:
Client:  PRIVMSG alice :hey are you there?\r\n

Server:
  → find client "alice"            → 401 if not found
  → send directly to alice:
     ":spider!user@host PRIVMSG alice :hey are you there?\r\n"
```

**→ Next:** [Operator Commands](#11-operator-commands)

---

## 11. Operator Commands

### KICK
```
Client:  KICK #general bob :bye!\r\n

params[0] = "#general"   channel
params[1] = "bob"        target to kick
params[2] = "bye!"       kick reason (optional)

Checks:
  → client in channel?             → 442
  → client is operator?            → 482
  → target in channel?             → 441

Success:
  → removeClient(bob)
  → broadcast: ":spider!user@host KICK #general bob :bye!\r\n"
```

### INVITE
```
Client:  INVITE bob #general\r\n

params[0] = "bob"        target to invite
params[1] = "#general"   channel

Checks:
  → client in channel?             → 442
  → client is operator?            → 482
  → target exists?                 → 401

Success:
  → addToInviteList(bob)
  → send to bob: ":spider!user@host INVITE bob #general\r\n"
  → send to spider: ":ircserv 341 spider bob #general\r\n"
```

### TOPIC
```
VIEW topic:
Client:  TOPIC #general\r\n
Server:  ":ircserv 332 spider #general :Current topic\r\n"

SET topic:
Client:  TOPIC #general :New topic here\r\n

Checks:
  → client in channel?             → 442
  → topicRestricted + not op?      → 482

Success:
  → setTopic("New topic here")
  → broadcast: ":spider!user@host TOPIC #general :New topic here\r\n"
```

**→ Next:** [MODE Command](#12-mode-command)

---

## 12. MODE Command

```
Client:  MODE #general +i\r\n
Client:  MODE #general -i\r\n
Client:  MODE #general +k password\r\n
Client:  MODE #general +o spider\r\n
Client:  MODE #general +l 10\r\n

Checks:
  → client in channel?             → 442
  → client is operator?            → 482

MODE FLAGS:
┌──────┬──────────────────────────────────────────────────┐
│ Flag │ Meaning                                           │
├──────┼──────────────────────────────────────────────────┤
│ +i   │ Set invite-only (only invited users can join)    │
│ -i   │ Remove invite-only                               │
│ +t   │ Only operators can change topic                  │
│ -t   │ Anyone can change topic                          │
│ +k   │ Set channel password (key)                       │
│ -k   │ Remove channel password                          │
│ +o   │ Give operator status to user                     │
│ -o   │ Take operator status from user                   │
│ +l   │ Set user limit                                   │
│ -l   │ Remove user limit                                │
└──────┴──────────────────────────────────────────────────┘

Success broadcast:
":spider!user@host MODE #general +i\r\n"
":spider!user@host MODE #general +k password\r\n"
":spider!user@host MODE #general +o bob\r\n"
```

**→ Next:** [Error Codes Reference](#13-error-codes-reference)

---

## 13. Error Codes Reference

### Registration Errors
```
431 ERR_NONICKNAMEGIVEN    → "No nickname given"
432 ERR_ERRONEUSNICKNAME   → "Erroneous nickname"
433 ERR_NICKNAMEINUSE      → "Nickname is already in use"
451 ERR_NOTREGISTERED      → "You have not registered"
461 ERR_NEEDMOREPARAMS     → "Not enough parameters"
462 ERR_ALREADYREGISTERED  → "You may not reregister"
464 ERR_PASSWDMISMATCH     → "Password incorrect"
```

### Channel Errors
```
403 ERR_NOSUCHCHANNEL      → "No such channel"
404 ERR_CANNOTSENDTOCHAN   → "Cannot send to channel"
471 ERR_CHANNELISFULL      → "Cannot join channel, full"
473 ERR_INVITEONLYCHAN     → "Cannot join, invite only"
475 ERR_BADCHANNELKEY      → "Cannot join, wrong key"
```

### User Errors
```
401 ERR_NOSUCHNICK         → "No such nick"
421 ERR_UNKNOWNCOMMAND     → "Unknown command"
```

### Permission Errors
```
441 ERR_USERNOTINCHANNEL   → "User not in channel"
442 ERR_NOTONCHANNEL       → "You are not on that channel"
482 ERR_CHANOPRIVSNEEDED   → "You are not channel operator"
```

### Success Replies
```
001 RPL_WELCOME            → "Welcome to the IRC server"
331 RPL_NOTOPIC            → "No topic is set"
332 RPL_TOPIC              → channel topic
341 RPL_INVITING           → invite confirmation
353 RPL_NAMREPLY           → list of channel members
366 RPL_ENDOFNAMES         → end of names list
```

### Reply Format
```cpp
// always follow this format:
":" + serverName + " " + code + " " + clientNick + " " + message + "\r\n"

// examples:
":ircserv 001 spider :Welcome to the IRC server\r\n"
":ircserv 433 spider bob :Nickname is already in use\r\n"
":ircserv 353 spider = #general :@alice bob spider\r\n"
```

**→ Next:** [What You Need From Developer 1](#14-what-you-need-from-developer-1)

---

## 14. What You Need From Developer 1

```
INTERFACE CONTRACT
══════════════════

Server must provide:
┌─────────────────────────────────────────────────────────┐
│  std::string                      getPassword() const   │
│  std::string                      getName()     const   │
│  std::map<int, Client*>&          getClients()          │
│  std::map<std::string, Channel*>& getChannels()         │
└─────────────────────────────────────────────────────────┘

Client must provide:
┌─────────────────────────────────────────────────────────┐
│  void sendMessage(const std::string& message)           │
└─────────────────────────────────────────────────────────┘

Developer 1 must call your code:
┌─────────────────────────────────────────────────────────┐
│  dispatch(client, line, server)                         │
│  called from PollLoop after complete IRC line received  │
└─────────────────────────────────────────────────────────┘
```

**→ Next:** [Phase Roadmap](#15-phase-roadmap)

---

## 15. Phase Roadmap

```
PHASE 1 — Registration ✓ DONE
├── Parser.hpp + Parser.cpp      ✓
├── Client.hpp + Client.cpp      ✓
├── Pass.hpp   + Pass.cpp        ✓
├── Nick.hpp   + Nick.cpp        ✓
├── User.hpp   + User.cpp        ✓
└── Dispatcher.hpp + Dispatcher.cpp ✓

PHASE 2 — Channels (NOW)
├── Channel.hpp                  ← completing now
├── Channel.cpp                  ← next
└── Join.hpp + Join.cpp          ← after channel

PHASE 3 — Messaging
└── Privmsg.hpp + Privmsg.cpp

PHASE 4 — Operator Commands
├── Topic.hpp  + Topic.cpp
├── Invite.hpp + Invite.cpp
└── Kick.hpp   + Kick.cpp

PHASE 5 — MODE
└── Mode.hpp + Mode.cpp
    ├── +i / -i   (invite only)
    ├── +t / -t   (topic restriction)
    ├── +k / -k   (channel password)
    ├── +o / -o   (operator status)
    └── +l / -l   (user limit)

PHASE 6 — Validation & Edge Cases
├── All error replies complete
├── Duplicate nickname on reconnect
├── Empty commands
├── Partial packet handling
└── Multiple clients stress test
```

**→ Next:** [File Structure](#16-file-structure)

---

## 16. File Structure

```
ft_irc/
├── include/
│   ├── Client.hpp              ← Client state + methods
│   ├── Channel.hpp             ← Channel state + methods
│   ├── Server.hpp              ← Server (Dev 1 owns)
│   ├── Parser.hpp              ← IRCMessage struct + Parser class
│   ├── Command.hpp             ← shared command types (if needed)
│   ├── Reply.hpp               ← reply formatting helpers
│   ├── commands/
│   │   ├── Pass.hpp
│   │   ├── Nick.hpp
│   │   ├── User.hpp
│   │   ├── Join.hpp
│   │   ├── Privmsg.hpp
│   │   ├── Kick.hpp
│   │   ├── Invite.hpp
│   │   ├── Topic.hpp
│   │   └── Mode.hpp
│   └── utils/
│       ├── NumericReplies.hpp  ← error code constants
│       └── Utils.hpp
│
├── src/
│   ├── parser/
│   │   ├── Parser.cpp          ← parse() + 4 helpers
│   │   └── Dispatcher.cpp      ← dispatch() → routes commands
│   ├── models/
│   │   ├── Client.cpp          ← client state logic
│   │   └── Channel.cpp         ← channel logic
│   ├── commands/
│   │   ├── Pass.cpp
│   │   ├── Nick.cpp
│   │   ├── User.cpp
│   │   ├── Join.cpp
│   │   ├── Privmsg.cpp
│   │   ├── Kick.cpp
│   │   ├── Invite.cpp
│   │   ├── Topic.cpp
│   │   └── Mode.cpp
│   ├── network/               ← Dev 1 owns all of these
│   │   ├── Socket.cpp
│   │   ├── Buffer.cpp
│   │   ├── PacketAssembler.cpp
│   │   ├── Receiver.cpp
│   │   └── Sender.cpp
│   ├── server/                ← Dev 1 owns all of these
│   │   ├── Server.cpp
│   │   ├── ServerInit.cpp
│   │   ├── PollLoop.cpp
│   │   ├── AcceptClient.cpp
│   │   ├── Disconnect.cpp
│   │   └── Shutdown.cpp
│   ├── replies/
│   │   └── Reply.cpp
│   └── utils/
│       ├── NumericReplies.cpp
│       └── Utils.cpp
│
├── tests/
│   ├── registration.sh         ← test PASS + NICK + USER
│   ├── basic_connection.sh
│   ├── join.sh
│   ├── privmsg.sh
│   ├── mode.sh
│   ├── parser.sh
│   ├── integration.sh
│   └── valgrind.sh
│
├── docs/
│   ├── architecture.md
│   ├── protocol_notes.md
│   └── evaluation_checklist.md
│
├── Makefile
├── README.md
└── plan.md
```

---

*This document covers the complete ft_irc project from Developer 2's perspective.*
*Keep this open while coding — every function and flow is mapped here.*
