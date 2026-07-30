# ft_irc — Testing Checklist (Server assumed finished)
> Practical, run-this-now test plan. Work top to bottom; check things off as they pass.

**Setup once:**
```bash
make re
./ircserv 6667 mypassword
```
Keep this terminal open and watch for crashes/segfaults throughout every test below.

---

## 1. Compilation & Basic Sanity

```bash
make            # should compile cleanly
make            # run again immediately, should NOT relink anything
make clean && make
make fclean && make re
```
- [ ] Compiles with no errors/warnings (`-Wall -Wextra -Werror`)
- [ ] Second `make` does nothing (no relinking)
- [ ] `clean` / `fclean` / `re` all work
- [ ] Binary is named `ircserv`
- [ ] Add `-std=c++98` to CXXFLAGS temporarily → still compiles, then revert

```bash
grep -rn "poll(" src/ include/
```
- [ ] Only **one** `poll()`/equivalent call in the whole codebase
- [ ] `fcntl(` calls grep clean — only `F_SETFL, O_NONBLOCK` pattern appears (if used at all)

---

## 2. Raw Protocol Testing with `nc`

Open a second terminal for each client.

### 2.1 Basic connect + registration
```bash
nc -C 127.0.0.1 6667
PASS mypassword
NICK spider
USER spider 0 * :Spider Real Name
```
- [ ] Receives `001 Welcome` after valid PASS/NICK/USER
- [ ] Wrong password → `464` and connection behaves per spec
- [ ] Missing PASS before NICK → `451`
- [ ] Empty NICK → `431`
- [ ] Invalid characters in NICK → `432`
- [ ] Duplicate NICK across two `nc` sessions → `433`
- [ ] Re-sending USER after registered → `462`

### 2.2 Partial / fragmented commands (mandatory test from subject)
```bash
nc -C 127.0.0.1 6667
```
Type `com`, press Ctrl+D, type `man`, press Ctrl+D, type `d` + Enter, press Ctrl+D.
- [ ] Server reconstructs the full line before processing (no crash, no garbage command)
- [ ] Try splitting a real command mid-stream, e.g. send `NIC` then wait 2s then send `K bob\r\n`
- [ ] Try sending two full commands in a single `send()` (e.g. `NICK a\r\nUSER a 0 * :A\r\n` pasted at once) → both processed in order

### 2.3 Empty / malformed input
- [ ] Send just `\r\n` → no crash, ignored or handled gracefully
- [ ] Send unknown command `FOO bar\r\n` → `421` unknown command, no crash
- [ ] Send a line with no trailing `\r\n` and never complete it → server doesn't hang, other clients unaffected

---

## 3. Multi-Client & Concurrency

Open 3–4 `nc` sessions (or mix `nc` + reference client) simultaneously.

- [ ] All clients can register independently at the same time
- [ ] Server remains responsive to all clients (none blocks another)
- [ ] `JOIN #general` from client A, then client B → both become members
- [ ] `PRIVMSG #general :hello` from A → B receives it, A does **not** receive own echo (unless spec requires it)
- [ ] Direct `PRIVMSG bob :hi` reaches only bob
- [ ] Kill one `nc` client (Ctrl+C) mid-session:
  - [ ] Server doesn't crash
  - [ ] Other clients keep working
  - [ ] A new client can still connect right after
- [ ] Kill `nc` after sending only half a command (no final `\r\n`):
  - [ ] No hang, no corrupted state, server keeps serving other clients
- [ ] Suspend a client with `Ctrl+Z`:
  - [ ] Flood `#general` from another client while it's suspended
  - [ ] `fg` the suspended client → it receives the buffered/queued messages correctly
  - [ ] No hang anywhere during this test

---

## 4. Reference IRC Client Testing

Connect with your chosen reference client (e.g. HexChat/Irssi/WeeChat — note which one).

- [ ] Connects to `ircserv` without any client-side error
- [ ] Authenticates with server password
- [ ] Sets nickname and username
- [ ] Joins a channel
- [ ] Sends/receives channel messages
- [ ] Sends/receives private messages
- [ ] Works simultaneously alongside an `nc` session in the same channel
- [ ] Channel member list (`/names` or equivalent) displays correctly, including `@` for operators

---

## 5. Channel & Operator Command Testing

Set up: client A joins `#test` first (becomes operator), client B joins after (regular user).

### TOPIC
- [ ] B views topic with no topic set → `331`
- [ ] A sets topic → broadcast to all members
- [ ] B views topic after set → `332`
- [ ] `MODE #test +t` then B tries to set topic → `482` (denied)
- [ ] A (operator) can still set topic after `+t`

### INVITE
- [ ] B (non-op) tries to `INVITE` someone → `482`
- [ ] A invites C to `#test` → C receives INVITE message, A receives `341`
- [ ] `MODE #test +i` (invite-only) → uninvited client D tries to JOIN → `473`
- [ ] Invited C can still JOIN after `+i`

### KICK
- [ ] B (non-op) tries to KICK someone → `482`
- [ ] A kicks B → B removed from channel, all members see KICK broadcast, B receives it too
- [ ] A tries to kick someone not in the channel → `441`

### MODE — full matrix
```
MODE #test +i
MODE #test -i
MODE #test +t
MODE #test -t
MODE #test +k secret
MODE #test -k
MODE #test +o bob
MODE #test -o bob
MODE #test +l 3
MODE #test -l
```
- [ ] Each flag toggles correctly and is confirmed via broadcast
- [ ] `+k` — joining without the key → `475`; with correct key → succeeds
- [ ] `+l 3` — 4th client trying to join → `471`; removing limit (`-l`) allows join again
- [ ] `+o`/`-o` — target's operator status actually changes (verify via NAMES `@` prefix and by testing they can/can't KICK afterward)
- [ ] Non-operator attempting any MODE change → `482`

---

## 6. Memory & Crash Testing

```bash
valgrind --leak-check=full --show-leak-kinds=all ./ircserv 6667 mypassword
```
Run a full session through valgrind (or `leaks`/`e_fence` on macOS):
- [ ] Connect multiple clients, register, join channels, chat, use all operator commands
- [ ] Disconnect clients abruptly (Ctrl+C) while under valgrind
- [ ] Shut down server (Ctrl+C / SIGINT) → clean exit
- [ ] `valgrind` reports **0 leaks, 0 errors** at exit
- [ ] Repeat with heavy flooding from multiple clients (stress test) → still 0 leaks, no crash

```bash
lsof -p $(pgrep ircserv)      # check fds aren't leaking
netstat -an | grep 6667       # check for stuck connections
```
- [ ] File descriptors are closed properly after client disconnects (no fd leak over time)
- [ ] No lingering `CLOSE_WAIT`/orphaned sockets after repeated connect/disconnect cycles

---

## 7. Edge Cases / Stress

- [ ] Connect and immediately disconnect, repeated 50+ times rapidly → server survives
- [ ] Send a very long single line (several KB) without `\r\n` → doesn't crash, either buffers or rejects gracefully
- [ ] Flood one client with hundreds of `PRIVMSG` in a tight loop → server stays responsive to other clients
- [ ] Open max reasonable number of simultaneous connections (e.g. 50–100) → all can register and chat

---

## 8. Final Pass Checklist

- [ ] All Section 1–7 items checked
- [ ] `valgrind` clean on a full mixed-command session
- [ ] Reference client + `nc` interop confirmed
- [ ] README up to date (build/run instructions, AI-usage disclosure)
- [ ] Ready to demo a live small modification if asked during evaluation
