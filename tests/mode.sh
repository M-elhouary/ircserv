#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "MODE Command Tests"
echo -e "  Syntax: MODE <channel> <flags> [<args...>]"
echo ""

register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: View current modes (no modes set)
subheader "Test 1: View current modes (no modes set)"
start_server
register_user "modeviewer1"
send "JOIN #modetest\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest\r\n")
assert_contains "View modes returns 324" "$RESPONSE" "324"
kill_server

# Test 2: Set +i (invite-only)
subheader "Test 2: Set +i (invite-only)"
start_server
register_user "modeop1"
send "JOIN #modetest2\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest2 +i\r\n")
assert_contains "MODE +i succeeds" "$RESPONSE" "MODE"
kill_server

# Test 3: Set +t (topic restricted)
subheader "Test 3: Set +t (topic restricted)"
start_server
register_user "modeop2"
send "JOIN #modetest3\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest3 +t\r\n")
assert_contains "MODE +t succeeds" "$RESPONSE" "MODE"
kill_server

# Test 4: Set +k (password)
subheader "Test 4: Set +k (password)"
start_server
register_user "modeop3"
send "JOIN #modetest4\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest4 +k secretpass\r\n")
assert_contains "MODE +k succeeds" "$RESPONSE" "MODE"
kill_server

# Test 5: Set +o (operator)
subheader "Test 5: Set +o (operator)"
start_server
register_user "modeop4"
send "JOIN #modetest5\r\n" > /dev/null
sleep 1
register_user "modeop4target"
RESPONSE=$(send "MODE #modetest5 +o modeop4target\r\n")
assert_contains "MODE +o succeeds" "$RESPONSE" "MODE"
kill_server

# Test 6: Set +l (user limit)
subheader "Test 6: Set +l (user limit)"
start_server
register_user "modeop5"
send "JOIN #modetest6\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest6 +l 10\r\n")
assert_contains "MODE +l succeeds" "$RESPONSE" "MODE"
kill_server

# Test 7: MODE without registration
subheader "Test 7: MODE without registration"
start_server
RESPONSE=$(send "MODE #general +i\r\n")
assert_contains "MODE without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 8: MODE nonexistent channel
subheader "Test 8: MODE nonexistent channel"
start_server
register_user "modeghost"
RESPONSE=$(send "MODE #ghost +i\r\n")
assert_contains "MODE nonexistent channel returns 403" "$RESPONSE" "403"
kill_server

# Test 9: MODE user not in channel
subheader "Test 9: MODE user not in channel"
start_server
register_user "modeoutsider"
RESPONSE=$(send "MODE #general +i\r\n")
assert_contains "MODE not in channel returns 442" "$RESPONSE" "442"
kill_server

# Test 10: MODE without operator status
subheader "Test 10: MODE without operator status"
start_server
register_user "moderegular"
send "JOIN #modetest7\r\n" > /dev/null
sleep 1
register_user "moderegular2"
send "JOIN #modetest7\r\n" > /dev/null
sleep 1
RESPONSE=$(send "MODE #modetest7 +i\r\n")
assert_contains "MODE without op returns 482" "$RESPONSE" "482"
kill_server

footer