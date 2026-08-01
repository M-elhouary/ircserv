#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "KICK Command Tests"
echo -e "  Syntax: KICK <channel> <nickname> [:<reason>]"
echo ""

register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: Kick user from channel (operator)
subheader "Test 1: Operator kicks user from channel"
start_server
register_user "kicker"
send "JOIN #kicktest\r\n" > /dev/null
sleep 1
register_user "kickee"
send "JOIN #kicktest\r\n" > /dev/null
sleep 1
RESPONSE=$(send "KICK #kicktest kickee :bye\r\n")
assert_contains "KICK succeeds" "$RESPONSE" "KICK"
kill_server

# Test 2: KICK without registration
subheader "Test 2: KICK without registration"
start_server
RESPONSE=$(send "KICK #general bob\r\n")
assert_contains "KICK without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 3: KICK no params
subheader "Test 3: KICK no params"
start_server
register_user "kicknoparam"
RESPONSE=$(send "KICK\r\n")
assert_contains "KICK no params returns 461" "$RESPONSE" "461"
kill_server

# Test 4: KICK nonexistent channel
subheader "Test 4: KICK nonexistent channel"
start_server
register_user "ghostkick"
RESPONSE=$(send "KICK #ghost bob\r\n")
assert_contains "KICK nonexistent channel returns 403" "$RESPONSE" "403"
kill_server

# Test 5: KICK user not in channel
subheader "Test 5: KICK user not in channel"
start_server
register_user "kicker2"
send "JOIN #kicktest2\r\n" > /dev/null
sleep 1
register_user "outsider2"
RESPONSE=$(send "KICK #kicktest2 outsider2\r\n")
assert_contains "KICK user not in channel returns 441" "$RESPONSE" "441"
kill_server

# Test 6: KICK without operator status
subheader "Test 6: KICK without operator status"
start_server
register_user "regular"
send "JOIN #kicktest3\r\n" > /dev/null
sleep 1
register_user "regular2"
send "JOIN #kicktest3\r\n" > /dev/null
sleep 1
RESPONSE=$(send "KICK #kicktest3 regular2\r\n")
assert_contains "KICK without op returns 482" "$RESPONSE" "482"
kill_server

footer