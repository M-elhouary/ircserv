# ft_irc Development Plan (2 Developers)

## Project Goal

Build a fully functional IRC server in **C++98** that satisfies all mandatory requirements of the 42 `ft_irc` project and, after successful completion, implements the optional bonus features.

---

# Team Members

* **Developer 1:** Networking & Server Infrastructure
* **Developer 2:** IRC Protocol & Business Logic

The objective is to split the work as evenly as possible while minimizing merge conflicts.

---

# High-Level Architecture

```
                    +----------------------+
                    |    IRC Client        |
                    +----------+-----------+
                               |
                         TCP Connection
                               |
                    +----------v-----------+
                    |      IRC Server       |
                    +----------+-----------+
                               |
               +---------------+----------------+
               |                                |
      Networking Layer                IRC Logic Layer
               |                                |
      poll() / sockets                Commands / Channels
               |                                |
               +---------------+----------------+
                               |
                     Client & Channel Models
```

---

# Core Classes

## Server

Responsibilities:

* Create socket
* Bind to port
* Listen for connections
* Configure non-blocking mode
* Execute poll() loop
* Accept new clients
* Remove disconnected clients
* Dispatch commands
* Maintain all connected clients
* Maintain all channels

---

## Client

Responsibilities:

* Socket file descriptor
* Nickname
* Username
* Authentication state
* Registration state
* Receive buffer
* Send helper functions
* Joined channels

---

## Channel

Responsibilities:

* Channel name
* Topic
* Members
* Operators
* Invite list
* Password (key)
* User limit
* Invite-only mode
* Topic restriction mode

---

## Command Parser

Responsibilities:

* Receive complete IRC line
* Split prefix/command/arguments
* Dispatch to correct handler

---

# Work Distribution

---

# Developer 1 (Networking & Infrastructure)

## Phase 1

* Project structure
* Makefile
* main()
* Server startup
* Argument parsing
* Socket creation

## Phase 2

* bind()
* listen()
* Non-blocking sockets
* poll() setup
* poll() event loop

## Phase 3

* Accept new clients
* Disconnect handling
* Client cleanup
* Signal handling

## Phase 4

* Receive data
* Buffer partial packets
* Detect complete commands
* Forward commands to parser

## Phase 5

* send()
* Broadcast helper
* Error handling
* Resource cleanup

## Phase 6

* Performance tests
* Multiple simultaneous clients
* Stress testing
* Memory leak checking

---

# Developer 2 (IRC Logic)

## Phase 1

Implement registration:

* PASS
* NICK
* USER

Registration should complete only when all required information has been provided.

## Phase 2

Implement channel creation.

Implement:

* JOIN

Maintain:

* Member lists
* Operator assignment

## Phase 3

Implement messaging:

* PRIVMSG to users
* PRIVMSG to channels

Broadcast messages to all channel members except sender.

## Phase 4

Implement:

* TOPIC
* INVITE
* KICK

Respect permissions.

## Phase 5

Implement MODE:

### i

Invite-only mode.

### t

Only operators may modify topic.

### k

Password-protected channel.

### o

Grant/remove operator status.

### l

Channel user limit.

## Phase 6

Implement all protocol validations:

* Missing parameters
* Invalid channel names
* Invalid permissions
* Authentication checks
* Duplicate nicknames

---

# Shared Responsibilities

Both developers should jointly design:

* Client class
* Channel class
* Server interfaces
* Parser format
* Error reply strategy
* Project coding conventions

Both developers review every pull request before merging.

---

# Git Workflow

## Main branch

```
main
```

Always stable.

## Feature branches

Developer 1:

```
feature/network
feature/poll
feature/socket
```

Developer 2:

```
feature/parser
feature/channels
feature/commands
```

Merge into `main` only after testing.

---

# Development Order

## Milestone 1

Server starts.

```
./ircserv <port> <password>
```

Accepts TCP connections.

---

## Milestone 2

Client authentication works.

Commands:

* PASS
* NICK
* USER

---

## Milestone 3

JOIN works.

Channels created dynamically.

---

## Milestone 4

PRIVMSG works.

Users communicate correctly.

---

## Milestone 5

Operator commands work.

* KICK
* INVITE
* TOPIC
* MODE

---

## Milestone 6

Server stable under multiple clients.

No crashes.

No blocking operations.

---

# Weekly Timeline

## Week 1

Developer 1

* Socket setup
* poll()
* Accept clients
* Receive loop

Developer 2

* Parser
* PASS
* NICK
* USER

Integration goal:

Users can successfully connect and register.

---

## Week 2

Developer 1

* Buffer reconstruction
* Send helpers
* Cleanup logic

Developer 2

* JOIN
* Channel storage
* PRIVMSG

Integration goal:

Users can join channels and exchange messages.

---

## Week 3

Developer 1

* Stress testing
* Robustness improvements
* Resource cleanup

Developer 2

* KICK
* INVITE
* TOPIC
* MODE

Integration goal:

Mandatory features complete.

---

## Week 4

Joint work:

* Fix bugs
* Improve error handling
* Test edge cases
* Verify protocol behavior
* Prepare README
* Prepare for evaluation

---

# Mandatory Completion Checklist

* [ ] Compiles with C++98
* [ ] Makefile works
* [ ] No unnecessary relinking
* [ ] Uses non-blocking I/O
* [ ] Uses a single poll() loop (or equivalent)
* [ ] Handles multiple clients
* [ ] PASS implemented
* [ ] NICK implemented
* [ ] USER implemented
* [ ] JOIN implemented
* [ ] PRIVMSG implemented
* [ ] Channel broadcasting works
* [ ] KICK implemented
* [ ] INVITE implemented
* [ ] TOPIC implemented
* [ ] MODE i implemented
* [ ] MODE t implemented
* [ ] MODE k implemented
* [ ] MODE o implemented
* [ ] MODE l implemented
* [ ] Handles partial packet reception
* [ ] Graceful client disconnects
* [ ] No memory leaks
* [ ] No crashes under normal operation

---

# Bonus Plan (Only After Mandatory Is Perfect)

## Developer 1

Implement file transfer support.

Tasks:

* Transfer protocol design
* Data streaming
* Large payload handling
* Reliability testing

## Developer 2

Implement IRC bot.

Possible features:

* Welcome messages
* Automatic help command
* Channel moderation
* Fun commands
* Simple administrative utilities

---

# Testing Strategy

Test with:

* Multiple IRC clients
* Invalid passwords
* Duplicate nicknames
* Rapid connect/disconnect cycles
* Empty commands
* Partial TCP packets
* Simultaneous messaging
* Large numbers of users in one channel
* Operator permission enforcement

---

# Daily Collaboration Rules

1. Commit frequently with descriptive messages.
2. Push changes at the end of each work session.
3. Review teammate code before merging.
4. Keep `main` buildable at all times.
5. Avoid changing another developer's files without discussion.
6. Run tests after every major feature.
7. Resolve merge conflicts immediately.

---

# Final Goal

Before starting any bonus work, ensure the mandatory implementation is complete, stable, well-tested, and fully understood by both developers. Only then proceed with bonus features while continuing integration testing and documentation updates.

