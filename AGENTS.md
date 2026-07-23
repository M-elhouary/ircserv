# AGENTS.md

## Project

IRC server in **C++98** for 42 school `ft_irc`. Mandatory features: PASS, NICK, USER, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE. Uses `poll()` for non-blocking I/O.

## Build & Run

```sh
make              # build ./ircserv
make re           # full rebuild
make clean        # remove .o files
make fclean       # remove .o files + binary
./ircserv <port> <password>   # e.g. ./ircserv 6667 password
```

Compiler: `c++ -Wall -Wextra -Werror -std=c++98`. No C++11 or later features allowed.

## Tests

All tests require `./ircserv` binary to exist. Run from project root.

```sh
./tests/parser.sh          # parser unit tests (stub)
./tests/integration.sh     # starts server on port 6667, tests connections via nc
./tests/valgrind.sh        # valgrind leak check (5s timeout)
```

CI (`ci.yml`) runs: `make re` → `parser.sh` → `integration.sh` → `valgrind.sh`. Test dependencies: `netcat-openbsd`, `valgrind`.

**Note:** Many test files and several headers are empty stubs (e.g. `Reply.hpp`, `NumericReplies.hpp`, `Colors.hpp`, `Utils.hpp`, `Command.hpp`).

## Architecture

```
src/main.cpp          → entrypoint, arg parsing, Server::init() + Server::run()
src/server/           → socket setup, poll loop, accept/disconnect
src/network/          → Buffer, PacketAssembler, Receiver, Sender, Socket
src/parser/           → Parser (IRC line → IRCMessage), Dispatcher (routes commands)
src/commands/         → command handlers (Pass, Nick, User, Join, Privmsg, Kick, Invite, Topic, Mode, Registration)
src/models/           → Client, Channel state
src/replies/          → reply formatting (stub)
src/utils/            → numeric reply constants, utilities (stubs)
```

**Key boundary:** `dispatch()` in `src/parser/Dispatcher.cpp` is the bridge between networking and IRC protocol. It receives raw lines from the poll loop and routes to command handlers.

**Current state:** Dispatcher only routes PASS, NICK, USER, JOIN. Privmsg, Kick, Invite, Topic, Mode are implemented as files but not wired into the dispatcher yet.

## Conventions

- All includes go through `include/ircserver.hpp` (umbrella header)
- New source files must be added to `SRCS` in `Makefile` or they won't compile
- IRC messages must end with `\r\n`
- Error replies follow IRC numeric format: `:ircserv <code> <nick> :<message>\r\n`
- Design references: `plan.md` (work plan), `ft_irc_map.md` (detailed function maps)
