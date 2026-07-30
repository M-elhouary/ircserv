# 1337 / 42 ft_irc Correction Cheat Sheet

> This document summarizes the official evaluation rules and checklist for the **ft_irc** project at 42/1337. It is intended as a quick reference for preparing for evaluations and performing peer corrections.

---

## Memory Leaks

Verify there are **no memory leaks**.

Recommended tools:

- valgrind
- leaks
- e_fence

Every heap allocation must be freed before program termination.

Memory leaks should trigger the corresponding evaluation flag.

---

# Mandatory Part

---

# 1. Basic Checks (20 pts)

## Compilation

Verify:

- Makefile exists
- Compiles correctly
- Uses C++
- Required compiler flags
- Executable name is correct

---

## poll()

Ask the students:

> How many `poll()` (or equivalent) calls exist?

Requirement:

- Exactly **one** poll()

---

## Event Loop

Verify that `poll()` is called before every:

- accept()
- recv()/read()
- send()/write()

---

## errno

After socket operations:

Do **not** rely on

```cpp
errno == EAGAIN
```

to trigger additional logic.

---

## fcntl()

Only this usage is allowed:

```cpp
fcntl(fd, F_SETFL, O_NONBLOCK);
```

Any other use of `fcntl()` is forbidden.

---

## Automatic Failure

If any Basic Check fails:

**Final Grade = 0**

---

# 2. Networking (20 pts)

Verify:

- Server starts correctly.
- Listens on every network interface.
- Uses the command-line port.
- Works with `nc`.
- Accepts commands.
- Sends replies.
- Ask which IRC client is the reference.
- Verify connection using that IRC client.
- Supports multiple simultaneous clients.
- Never blocks.
- Works simultaneously with IRC client and nc.
- Join channels.
- Messages sent in a channel reach every member.

---

# 3. Networking Special Cases (20 pts)

## Partial Commands

Using nc:

Send commands in pieces.

Verify:

- Server buffers correctly.
- Other clients continue working.

---

## Unexpected Client Disconnect

Kill an IRC client unexpectedly.

Verify:

- Server survives.
- Other clients continue working.
- New clients can still connect.

---

## Interrupted nc

Kill nc after sending only half a command.

Verify:

- No blocked server.
- No corrupted state.

---

## Suspended Client

Suspend a client using:

```bash
Ctrl + Z
```

While suspended:

Flood the channel from another client.

Verify:

- Server never hangs.
- Buffered messages are processed correctly after resume.
- No memory leaks.

---

# 4. Client Commands (20 pts)

Using:

- nc
- Reference IRC client

Verify:

Authentication

- PASS
- NICK
- USER

Joining

- JOIN

Messaging

- PRIVMSG

PRIVMSG must work correctly with every valid parameter.

---

# 5. Channel Operator Commands (20 pts)

Using:

- nc
- IRC client

Verify that:

Regular users **cannot** perform operator actions.

Operators **can** perform them.

Typical operator features include:

- KICK
- INVITE
- TOPIC
- MODE

Remove one point for each missing feature.

---

# Bonus

Evaluate **only if**:

- Mandatory part is perfect.
- Error handling is robust.
- Invalid usage is handled correctly.

Otherwise:

**Ignore all bonus points.**

---

# Bonus 1 — File Transfer (12.5 pts)

Verify:

File transfer works using the reference IRC client.

---

# Bonus 2 — IRC Bot (12.5 pts)

Verify:

A small IRC bot works correctly.

---

# Quick Evaluation Checklist

## Basic

- [ ] Makefile
- [ ] C++
- [ ] Correct executable
- [ ] One poll()
- [ ] poll before every socket operation
- [ ] Correct fcntl()
- [ ] No errno misuse

---

## Networking

- [ ] Listens correctly
- [ ] nc works
- [ ] IRC client works
- [ ] Multiple clients
- [ ] Non-blocking
- [ ] Channel messaging

---

## Special Networking

- [ ] Partial commands
- [ ] Client crash
- [ ] Partial nc disconnect
- [ ] Suspended client
- [ ] No hangs
- [ ] No leaks

---

## Commands

- [ ] PASS
- [ ] NICK
- [ ] USER
- [ ] JOIN
- [ ] PRIVMSG

---

## Operator

- [ ] KICK
- [ ] INVITE
- [ ] TOPIC
- [ ] MODE
- [ ] Permission checks

---

## Memory

- [ ] No leaks
- [ ] No crashes
- [ ] No segfaults

---

## Bonus

- [ ] File Transfer
- [ ] IRC Bot

---

# Recommended Testing Tools

- nc (netcat)
- HexChat (or the team's reference IRC client)
- valgrind
- leaks (macOS)
- e_fence
- lsof
- netstat / ss
- strace (Linux)