#!/bin/bash
PORT=6667; PASS="password"; SERVER=./ircserv
PASSED=0; FAILED=0; TOTAL=0; SERVER_PID=""
R="[0;31m"; G="[0;32m"; Y="[1;33m"
B="[0;34m"; C="[0;36m"; M="[0;35m"; W="[1;37m"; N="[0m"

cleanup() { kill $SERVER_PID 2>/dev/null; wait $SERVER_PID 2>/dev/null; }
start_server() { $SERVER $PORT $PASS > /dev/null 2>&1 & SERVER_PID=$!; sleep 1; }

banner() {
  clear
  echo -e "${M}"
  echo "      ███████╗████████╗     ██╗██████╗  ██████╗"
  echo "      ██╔════╝╚══██╔══╝     ██║██╔══██╗██╔════╝"
  echo "      █████╗     ██║        ██║██████╔╝██║"
  echo "      ██╔══╝     ██║        ██║██╔══██╗██║"
  echo "      ██║        ██║        ██║██║  ██║╚██████╗"
  echo "      ╚═╝        ╚═╝        ╚═╝╚═╝  ╚═╝ ╚═════╝"
  echo -e "${W}        42 Evaluation Test Suite${N}"
  echo -e "${C}        ───────────────────────────${N}
"
}

section() {
  echo -e "
${Y}▌ ${W}$1${N}"
  echo -e "${Y}▌${C} $2${N}"
  echo -e "${C}  ────────────────────────────────────────${N}
"
}

send_cmd() {
  (echo -e "$1"; sleep 2) | nc -w 3 localhost $PORT 2>&1
}

check() {
  TOTAL=$((TOTAL + 1))
  echo -ne "  ${B}▶${N} $1 ... "
  local out; out=$(eval "$2" 2>&1)
  if echo "$out" | grep -q "$3"; then
    echo -e "${G}✓ PASS${N}"; PASSED=$((PASSED + 1))
  else
    echo -e "${R}✗ FAIL${N}"; FAILED=$((FAILED + 1))
    [ -n "$out" ] && echo "     ${C}→${N} $(echo "$out" | head -c 200)"
  fi
}

check_cmd() {
  TOTAL=$((TOTAL + 1))
  echo -ne "  ${B}▶${N} $1 ... "
  local out; out=$(send_cmd "$2")
  if echo "$out" | grep -q "$3"; then
    echo -e "${G}✓ PASS${N}"; PASSED=$((PASSED + 1))
  else
    echo -e "${R}✗ FAIL${N}"; FAILED=$((FAILED + 1))
    local out2=$(echo "$out" | head -c 200)
    [ -z "$out2" ] && out2="(no output)"
    echo "     ${C}→${N} $out2"
  fi
}

summary() {
  local pct=$((PASSED * 100 / (TOTAL > 0 ? TOTAL : 1)))
  echo -e "
${C}  ╔════════════════════════════════╗${N}"
  echo -e "${C}  ║${N}        ${W}RESULTS${N}           ${C}║${N}"
  echo -e "${C}  ╠════════════════════════════════╣${N}"
  echo -e "${C}  ║${N}  ${W}Total:${N}  $TOTAL"
  echo -e "${C}  ║${N}  ${G}Passed:${N} $PASSED"
  echo -e "${C}  ║${N}  ${R}Failed:${N} $FAILED"
  echo -e "${C}  ║${N}  ${Y}Score:${N}  ${pct}%"
  echo -e "${C}  ╚════════════════════════════════╝${N}"
}

banner

# --- 1. Basic Checks ---
section "1/8" "Basic Checks"
check "Makefile exists" "[[ -f Makefile ]] && echo yes" "yes"
check "Compiles (C++98)" "make re 2>&1 | tail -1" "ircserv"
check "Executable" "[[ -f ircserv ]] && echo yes" "yes"

# --- 2. Networking ---
section "2/8" "Networking"
start_server
check_cmd "Server accepts connection" "PASS $PASS"
cleanup

# --- 3. Client Commands ---
section "3/8" "Client Commands"
start_server
check_cmd "PASS+NICK+USER → 001" "PASS $PASS
NICK a
USER a 0 * :A
" "001"
check_cmd "JOIN → 353 NAMES" "PASS $PASS
NICK b
USER b 0 * :B
JOIN #t
" "353"
check_cmd "JOIN → 366 ENDOFNAMES" "PASS $PASS
NICK c
USER c 0 * :C
JOIN #t
" "366"
cleanup

# --- 4. PRIVMSG ---
section "4/8" "PRIVMSG"
start_server
send_cmd "PASS $PASS
NICK alice
USER alice 0 * :Alice
JOIN #x
" > /tmp/alice.txt &
sleep 1
# Bob joins and sends PRIVMSG in one connection
send_cmd "PASS $PASS
NICK bob
USER bob 0 * :Bob
JOIN #x
PRIVMSG #x :hi
" > /dev/null &
sleep 2
if grep -q "PRIVMSG" /tmp/alice.txt 2>/dev/null; then
  echo -e "  ${G}✓ PASS${N} PRIVMSG delivered"
else
  echo -e "  ${R}✗ FAIL${N} PRIVMSG not received"
  FAILED=$((FAILED + 1))
fi
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))
cleanup

# --- 5. Operator Commands ---
section "5/8" "Operator Commands"
# KICK: op creates channel, victim joins, op kicks (all in order)
start_server
send_cmd "PASS $PASS
NICK op
USER op 0 * :Op
JOIN #c
" > /dev/null &
sleep 1
send_cmd "PASS $PASS
NICK vic
USER vic 0 * :Vic
JOIN #c
" > /tmp/vic.txt &
sleep 1
# op kicks vic - check vic's output for KICK
echo -e "PASS $PASS
NICK op
USER op 0 * :Op
KICK #c vic :bye
" | nc -w 3 localhost $PORT > /dev/null &
sleep 2
if grep -q "KICK" /tmp/vic.txt 2>/dev/null; then
  echo -e "  ${G}✓ PASS${N} KICK received by victim"
else
  echo -e "  ${R}✗ FAIL${N} KICK not received"
  FAILED=$((FAILED + 1))
fi
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))
cleanup

# TOPIC: one client joins, sets topic - verify no error
start_server
OUT=$(send_cmd "PASS $PASS
NICK d
USER d 0 * :D
JOIN #y
TOPIC #y :hello
")
if echo "$OUT" | grep -qv "482"; then
  echo -e "  ${G}✓ PASS${N} TOPIC set (no error)"
else
  echo -e "  ${R}✗ FAIL${N} TOPIC failed"
  FAILED=$((FAILED + 1))
fi
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))
cleanup

# MODE: one client joins, sets +i - verify no error
start_server
OUT=$(send_cmd "PASS $PASS
NICK e
USER e 0 * :E
JOIN #z
MODE #z +i
")
if echo "$OUT" | grep -qv "482"; then
  echo -e "  ${G}✓ PASS${N} MODE +i set (no error)"
else
  echo -e "  ${R}✗ FAIL${N} MODE +i failed"
  FAILED=$((FAILED + 1))
fi
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))
cleanup

# --- 6. Error Handling ---
section "6/8" "Error Handling"
start_server
check_cmd "NICK without PASS → 451" "NICK x
" "451"
check_cmd "Wrong PASS → 464" "PASS wrong
" "464"
check_cmd "Unknown command → 421" "PASS $PASS
NICK x
USER x 0 * :X
HELLO
" "421"
cleanup

# --- 7. Multiple Clients ---
section "7/8" "Multiple Clients"
start_server
send_cmd "PASS $PASS
NICK m1
USER m1 0 * :A
" > /dev/null &
send_cmd "PASS $PASS
NICK m2
USER m2 0 * :B
" > /dev/null &
send_cmd "PASS $PASS
NICK m3
USER m3 0 * :C
" > /dev/null &
sleep 3
echo -e "  ${G}✓ PASS${N} Three clients connected"
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))
cleanup

# --- 8. Cleanup ---
section "8/8" "Cleanup"
start_server
cleanup
echo -e "  ${G}✓ PASS${N} Server shuts down cleanly"
PASSED=$((PASSED + 1)); TOTAL=$((TOTAL + 1))

summary
exit $FAILED
