# ft_irc

An IRC (Internet Relay Chat) server written in **C++98**, fully compliant with the 42 school project requirements. Supports multiple simultaneous clients, channel management, and operator commands.

## Features

### Core Commands
| Command | Description |
|---------|-------------|
| `PASS` | Authenticate with the server password |
| `NICK` | Set or change your nickname |
| `USER` | Set your username and real name |
| `JOIN` | Join a channel (creates it if it doesn't exist) |
| `PRIVMSG` | Send a message to a channel or private user |
| `KICK` | Remove a user from a channel (operator only) |
| `INVITE` | Invite a user to a channel (operator only) |
| `TOPIC` | View or set the channel topic |
| `MODE` | Change channel modes (`+i`, `+t`, `+k`, `+o`, `+l`) |

### Channel Modes
| Mode | Description |
|------|-------------|
| `+i` | Invite-only channel |
| `+t` | Only operators can change the topic |
| `+k` | Set a channel password (key) |
| `+o` | Grant/revoke operator status |
| `+l` | Set maximum user limit |

### Technical Details
- Non-blocking I/O with `poll()` event loop
- Partial TCP packet handling (correct `\r\n` line assembly)
- Graceful client disconnect handling
- Dynamic channel creation and destruction
- Operator permission enforcement

## Requirements

- C++ compiler with C++98 support
- `make`
- `nc` (netcat) for testing

## Build

```bash
# Compile the server
make

# Rebuild from scratch
make re

# Clean object files
make clean

# Clean everything
make fclean
```

## Usage

```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6667 mypassword
```

### Connecting with an IRC Client

Most IRC clients support custom server connections:

1. Open your IRC client (e.g., HexChat, irssi, NetCat)
2. Add a new server:
   - **Hostname:** `localhost`
   - **Port:** `6667`
   - **Password:** `mypassword`
3. Connect and register:
   ```
   /pass mypassword
   /nick yourname
   /user yourname 0 * :Your Name
   ```

### Connecting with NetCat

```bash
# Connect to the server
nc localhost 6667

# Send commands manually
PASS mypassword
NICK testuser
USER testuser 0 * :Test User
JOIN #general
PRIVMSG #general :Hello everyone!
```

## Project Structure

```
.
├── include/
│   ├── Client.hpp              # Client state and methods
│   ├── Channel.hpp             # Channel state and methods
│   ├── Server.hpp              # Server class
│   ├── Parser.hpp              # IRC message parser
│   ├── Dispatcher.hpp          # Command dispatcher
│   └── commands/
│       ├── Pass.hpp            # PASS command
│       ├── Nick.hpp            # NICK command
│       ├── User.hpp            # USER command
│       ├── Join.hpp            # JOIN command
│       ├── Privmsg.hpp         # PRIVMSG command
│       ├── Kick.hpp            # KICK command
│       ├── Invite.hpp          # INVITE command
│       ├── Topic.hpp           # TOPIC command
│       └── Mode.hpp            # MODE command
├── src/
│   ├── main.cpp                # Entry point
│   ├── server/
│   │   ├── Server.cpp          # Server core
│   │   ├── ServerInit.cpp      # Socket setup and poll loop
│   │   ├── AcceptClient.cpp    # Client connection handling
│   │   └── Disconnect.cpp      # Client disconnect handling
│   ├── commands/
│   │   ├── Pass.cpp            # PASS implementation
│   │   ├── Nick.cpp            # NICK implementation
│   │   ├── User.cpp            # USER implementation
│   │   ├── Join.cpp            # JOIN implementation
│   │   ├── Privmsg.cpp         # PRIVMSG implementation
│   │   ├── Kick.cpp            # KICK implementation
│   │   ├── Invite.cpp          # INVITE implementation
│   │   ├── Topic.cpp           # TOPIC implementation
│   │   └── Mode.cpp            # MODE implementation
│   ├── models/
│   │   ├── Client.cpp          # Client model
│   │   └── Channel.cpp         # Channel model
│   ├── network/
│   │   ├── Buffer.cpp          # Receive buffer management
│   │   └── Receiver.cpp        # Data reception
│   └── parser/
│       ├── Parser.cpp          # IRC message parsing
│       └── Dispatcher.cpp      # Command routing
├── tests/
│   └── basic_connection.sh     # Connection and registration tests
├── Makefile
└── README.md
```

## Testing

### Run Tests

```bash
# Run all basic connection tests
bash tests/basic_connection.sh
```

### Manual Testing

Start the server and connect with multiple clients:

```bash
# Terminal 1 - Start server
./ircserv 6667 password

# Terminal 2 - Client 1
nc localhost 6667
PASS password
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :Hi from Alice!

# Terminal 3 - Client 2
nc localhost 6667
PASS password
NICK bob
USER bob 0 * :Bob
JOIN #general
PRIVMSG #general :Hi from Bob!
```

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 001 | RPL_WELCOME | Welcome message |
| 331 | RPL_NOTOPIC | No topic set |
| 332 | RPL_TOPIC | Channel topic |
| 341 | RPL_INVITING | Invite confirmation |
| 353 | RPL_NAMREPLY | Channel member list |
| 366 | RPL_ENDOFNAMES | End of names list |
| 401 | ERR_NOSUCHNICK | No such nick/channel |
| 403 | ERR_NOSUCHCHANNEL | No such channel |
| 404 | ERR_CANNOTSENDTOCHAN | Cannot send to channel |
| 421 | ERR_UNKNOWNCOMMAND | Unknown command |
| 431 | ERR_NONICKNAMEGIVEN | No nickname given |
| 432 | ERR_ERRONEUSNICKNAME | Erroneous nickname |
| 433 | ERR_NICKNAMEINUSE | Nickname already in use |
| 441 | ERR_USERNOTINCHANNEL | User not in channel |
| 442 | ERR_NOTONCHANNEL | You're not on that channel |
| 451 | ERR_NOTREGISTERED | You have not registered |
| 461 | ERR_NEEDMOREPARAMS | Not enough parameters |
| 462 | ERR_ALREADYREGISTERED | You may not reregister |
| 464 | ERR_PASSWDMISMATCH | Password incorrect |
| 471 | ERR_CHANNELISFULL | Channel is full |
| 473 | ERR_INVITEONLYCHAN | Invite-only channel |
| 475 | ERR_BADCHANNELKEY | Wrong channel key |
| 482 | ERR_CHANOPRIVSNEEDED | You're not channel operator |

## Authors

**Developer 1** - Networking & Server Infrastructure
- Socket management and poll() event loop
- Non-blocking I/O and partial packet handling
- Client connection and disconnection

**Developer 2** - IRC Protocol & Business Logic
- Command parsing and dispatching
- Channel and client state management
- All IRC command implementations
