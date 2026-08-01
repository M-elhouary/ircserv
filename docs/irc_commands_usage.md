# ft_irc — Command Usage Guide
> How to trigger every command and every case (success + all errors)
> Test with: `nc localhost <port>` or `nc -C localhost <port>` (nc adds \r\n with -C on some systems; otherwise type `PASS x\r` manually or use irssi/netcat with a script)

---

## Setup

Start the server:
```bash
./ircserv 6667 mypassword
```

Connect a client:
```bash
nc localhost 6667
```
> Note: every line you send must end in `\r\n`. Plain `nc` only sends `\n`. Either patch your parser to accept a lone `\n` during testing, or use a tool that sends `\r\n` (e.g. `irssi`, `netcat -C`, or a small Python script with `sock.send(b"PASS x\r\n")`).

---

## 1. PASS

**Syntax:** `PASS <password>`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success | `PASS mypassword` | not yet authenticated | `_authenticated = true`, no reply |
| Already registered | `PASS mypassword` | client already sent a valid PASS before | `462 :You may not reregister` |
| No params | `PASS` | — | `461 :Not enough parameters` |
| Wrong password | `PASS wrongpass` | — | `464 :Password incorrect` (+ disconnect per spec) |

```
PASS mypassword\r\n
```

---

## 2. NICK

**Syntax:** `NICK <nickname>`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success | `NICK spider` | authenticated | nickname set, `_nicknameReceived = true` |
| Not authenticated | `NICK spider` | PASS not sent/accepted yet | `451 :You have not registered` |
| No params | `NICK` | — | `431 :No nickname given` |
| Invalid nickname | `NICK 1abc` or `NICK a b` or `NICK ab$c` | starts with digit, contains space/illegal char | `432 :Erroneous nickname` |
| Nickname taken | `NICK bob` | "bob" already used by another connected client | `433 :Nickname is already in use` |

```
NICK spider\r\n
```
To test "taken": open a second `nc` connection, complete its PASS, then try the same nickname as client 1.

---

## 3. USER

**Syntax:** `USER <username> <hostname> <servername> :<realname>`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success | `USER spider 0 * :Spider Real Name` | authenticated | `_registered = true`, sends `001 Welcome` |
| Not authenticated | `USER spider 0 * :Spider` | PASS not done | `451 :You have not registered` |
| Already registered | `USER spider 0 * :Spider` | USER already sent once | `462 :You may not reregister` |
| No params | `USER` or `USER spider` (missing trailing) | fewer than 4 params | `461 :Not enough parameters` |

```
USER spider 0 * :Spider Real Name\r\n
```

**Full registration sequence:**
```
PASS mypassword\r\n
NICK spider\r\n
USER spider 0 * :Spider Real Name\r\n
```
Expected reply: `:ircserv 001 spider :Welcome to the IRC server\r\n`

---

## 4. JOIN

**Syntax:** `JOIN <channel>[,<channel2>...] [<key>[,<key2>...]]`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success — create new channel | `JOIN #general` | channel doesn't exist yet | channel created, joiner becomes operator, gets JOIN + 353 + 366 |
| Success — join existing | `JOIN #general` | channel exists, no restrictions | joins as regular member |
| Not registered | `JOIN #general` | client hasn't completed PASS/NICK/USER | `451 :You have not registered` |
| Invalid channel name | `JOIN general` (no `#`) | — | `403 :No such channel` |
| Channel full | `JOIN #general` | `_userLimit` reached (+l mode set) | `471 :Cannot join channel, full` |
| Invite-only, not invited | `JOIN #general` | channel has `+i`, client not in invite list | `473 :Cannot join, invite only` |
| Wrong key | `JOIN #general wrongkey` | channel has `+k` password set | `475 :Cannot join, wrong key` |
| Correct key | `JOIN #general mypassword` | channel `+k mypassword` | joins successfully |

```
JOIN #general\r\n
```
With a key:
```
JOIN #general secretkey\r\n
```

Expected success replies:
```
:spider!user@host JOIN #general\r\n
:ircserv 353 spider = #general :@spider\r\n
:ircserv 366 spider #general :End of /NAMES list\r\n
```

---

## 5. PRIVMSG

**Syntax:** `PRIVMSG <target> :<message>`
`<target>` is either a channel (`#name`) or a nickname.

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success — to channel | `PRIVMSG #general :hello everyone` | sender registered + in channel | message broadcast to all members except sender |
| Success — to user | `PRIVMSG bob :hey` | sender registered, "bob" connected | message delivered to bob only |
| Not registered | `PRIVMSG #general :hi` | sender not registered | `451 :You have not registered` |
| No params | `PRIVMSG` or `PRIVMSG #general` (no message) | — | `461 :Not enough parameters` |
| Channel doesn't exist | `PRIVMSG #ghost :hi` | channel not found | `403 :No such channel` |
| Not in channel | `PRIVMSG #general :hi` | sender registered but not a member of #general | `404 :Cannot send to channel` |
| User doesn't exist | `PRIVMSG nobody :hi` | nickname not found | `401 :No such nick` |

```
PRIVMSG #general :hello everyone\r\n
PRIVMSG bob :are you there?\r\n
```

Expected broadcast format seen by other members:
```
:spider!user@host PRIVMSG #general :hello everyone\r\n
```

---

## 6. KICK

**Syntax:** `KICK <channel> <nickname> [:<reason>]`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success | `KICK #general bob :bye!` | kicker is operator, both in channel | bob removed, `KICK` broadcast to channel |
| Not registered | `KICK #general bob` | — | `451 :You have not registered` |
| No params | `KICK` or `KICK #general` | missing channel or nick | `461 :Not enough parameters` |
| Channel doesn't exist | `KICK #ghost bob` | — | `403 :No such channel` |
| Kicker not in channel | `KICK #general bob` | kicker not a member | `442 :You are not on that channel` |
| Kicker not operator | `KICK #general bob` | kicker is a regular member | `482 :You are not channel operator` |
| Target not in channel | `KICK #general alice` | alice not a member of #general | `441 :User not in channel` |

```
KICK #general bob :breaking the rules\r\n
```
Without a reason (optional param):
```
KICK #general bob\r\n
```

---

## 7. INVITE

**Syntax:** `INVITE <nickname> <channel>`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Success | `INVITE bob #general` | inviter is operator/member, bob exists | bob added to invite list, both notified |
| Not registered | `INVITE bob #general` | — | `451 :You have not registered` |
| No params | `INVITE` or `INVITE bob` | missing nick or channel | `461 :Not enough parameters` |
| Channel doesn't exist | `INVITE bob #ghost` | — | `403 :No such channel` |
| Inviter not in channel | `INVITE bob #general` | inviter not a member | `442 :You are not on that channel` |
| Inviter not operator | `INVITE bob #general` | channel is invite-only and inviter isn't op (spec-dependent — check your impl) | `482 :You are not channel operator` |
| Target doesn't exist | `INVITE ghostuser #general` | nickname not connected | `401 :No such nick` |

```
INVITE bob #general\r\n
```

Expected replies:
```
:spider!user@host INVITE bob #general\r\n     (sent to bob)
:ircserv 341 spider bob #general\r\n           (sent to spider, confirmation)
```

---

## 8. TOPIC

**Syntax:** `TOPIC <channel> [:<new topic>]`

| Case | Command | Precondition | Result |
|---|---|---|---|
| View topic (set) | `TOPIC #general` | topic already set | `332 :<current topic>` |
| View topic (none set) | `TOPIC #general` | no topic set yet | `331 :No topic is set` |
| Set topic — success | `TOPIC #general :New topic here` | client in channel, allowed to set | topic updated, broadcast to all members |
| Not registered | `TOPIC #general` | — | `451 :You have not registered` |
| Channel doesn't exist | `TOPIC #ghost` | — | `403 :No such channel` |
| Client not in channel | `TOPIC #general :new` | client not a member | `442 :You are not on that channel` |
| Topic restricted, not op | `TOPIC #general :new` | channel `+t`, client not operator | `482 :You are not channel operator` |

```
TOPIC #general\r\n
TOPIC #general :Welcome to the general channel!\r\n
```

---

## 9. MODE

**Syntax:** `MODE <channel> <flags> [<args...>]`

| Case | Command | Precondition | Result |
|---|---|---|---|
| Not registered | `MODE #general +i` | — | `451 :You have not registered` |
| Channel doesn't exist | `MODE #ghost +i` | — | `403 :No such channel` |
| Not operator | `MODE #general +i` | client is not a channel operator | `482 :You are not channel operator` |

### Mode flags — one row per flag

| Flag | Command | Meaning | Notes |
|---|---|---|---|
| `+i` | `MODE #general +i` | set invite-only | now JOIN requires invite |
| `-i` | `MODE #general -i` | remove invite-only | anyone can JOIN again |
| `+t` | `MODE #general +t` | only operators can change topic | |
| `-t` | `MODE #general -t` | anyone can change topic | |
| `+k` | `MODE #general +k secret` | set channel password (key) | requires 1 arg |
| `-k` | `MODE #general -k` | remove channel password | |
| `+o` | `MODE #general +o bob` | give bob operator status | requires target nickname, target must be member |
| `-o` | `MODE #general -o bob` | remove bob's operator status | |
| `+l` | `MODE #general +l 10` | set user limit to 10 | requires numeric arg |
| `-l` | `MODE #general -l` | remove user limit | |

**Combined flags** (bonus/edge case — check if your parser handles this):
```
MODE #general +ok secretpass bob\r\n
```
This sets `+o bob` and `+k secretpass` in one line — real IRC servers support multiple flags per MODE call with args consumed in order. Confirm whether your Mode.cpp handles multi-flag strings or only single-flag calls; if only single-flag, document that as a known limitation.

Expected broadcast on success:
```
:spider!user@host MODE #general +i\r\n
:spider!user@host MODE #general +o bob\r\n
```

---

## 10. Quick End-to-End Test Script (two clients)

**Terminal 1 (operator "spider"):**
```
PASS mypassword\r\n
NICK spider\r\n
USER spider 0 * :Spider\r\n
JOIN #general\r\n
TOPIC #general :Welcome!\r\n
MODE #general +t\r\n
```

**Terminal 2 ("bob"):**
```
PASS mypassword\r\n
NICK bob\r\n
USER bob 0 * :Bob\r\n
JOIN #general\r\n
PRIVMSG #general :hi everyone\r\n
```

**Back in Terminal 1:**
```
KICK #general bob :testing kick\r\n
```

This single flow exercises: registration (all 3 commands), channel creation, topic set, mode change, cross-client PRIVMSG, and operator-only KICK — hitting most of the success paths in one pass. To hit the error paths, deliberately break preconditions (wrong password, duplicate nick, message before registering, non-op trying MODE/KICK/INVITE, etc.) using the tables above.

---

## 11. Error Code Cheat Sheet (all codes referenced above)

```
401 ERR_NOSUCHNICK          No such nick
403 ERR_NOSUCHCHANNEL       No such channel
404 ERR_CANNOTSENDTOCHAN    Cannot send to channel
431 ERR_NONICKNAMEGIVEN     No nickname given
432 ERR_ERRONEUSNICKNAME    Erroneous nickname
433 ERR_NICKNAMEINUSE       Nickname is already in use
441 ERR_USERNOTINCHANNEL    User not in channel
442 ERR_NOTONCHANNEL        You're not on that channel
451 ERR_NOTREGISTERED       You have not registered
461 ERR_NEEDMOREPARAMS      Not enough parameters
462 ERR_ALREADYREGISTERED   You may not reregister
464 ERR_PASSWDMISMATCH      Password incorrect
471 ERR_CHANNELISFULL       Cannot join channel, full
473 ERR_INVITEONLYCHAN      Cannot join, invite only
475 ERR_BADCHANNELKEY       Cannot join, wrong key
482 ERR_CHANOPRIVSNEEDED    You're not channel operator
```
