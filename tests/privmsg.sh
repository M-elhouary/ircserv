#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "PRIVMSG Command Tests"
echo -e "  Syntax: PRIVMSG <target> :<message>"
echo ""

register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: PRIVMSG to channel (sender in channel)
subheader "Test 1: PRIVMSG to channel"
start_server
register_user "privmsguser"
send "JOIN #chat\r\n" > /dev/null
sleep 1
RESPONSE=$(send "PRIVMSG #chat :Hello channel\r\n")
assert_contains "PRIVMSG to channel succeeds" "$RESPONSE" "Hello channel"
kill_server

# Test 2: PRIVMSG to user
subheader "Test 2: PRIVMSG to user"
start_server
register_user "sender1"
send "JOIN #chat\r\n" > /dev/null
sleep 1
register_user "receiver1"
send "JOIN #chat\r\n" > /dev/null
sleep 1
RESPONSE=$(send "PRIVMSG receiver1 :Hello privately\r\n")
assert_contains "PRIVMSG to user succeeds" "$RESPONSE" "Hello privately"
kill_server

# Test 3: PRIVMSG without registration
subheader "Test 3: PRIVMSG without registration"
start_server
RESPONSE=$(send "PRIVMSG #general :hi\r\n")
assert_contains "PRIVMSG without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 4: PRIVMSG no params
subheader "Test 4: PRIVMSG no params"
start_server
register_user "noparammsg"
RESPONSE=$(send "PRIVMSG\r\n")
assert_contains "PRIVMSG no params returns 461" "$RESPONSE" "461"
kill_server

# Test 5: PRIVMSG to nonexistent channel
subheader "Test 5: PRIVMSG to nonexistent channel"
start_server
register_user "ghostchan"
RESPONSE=$(send "PRIVMSG #ghost :hi\r\n")
assert_contains "PRIVMSG to nonexistent channel returns 403" "$RESPONSE" "403"
kill_server

# Test 6: PRIVMSG to channel user not in
subheader "Test 6: PRIVMSG to channel user not in"
start_server
register_user "outsider"
RESPONSE=$(send "PRIVMSG #general :hi\r\n")
assert_contains "PRIVMSG to channel not in returns 404" "$RESPONSE" "404"
kill_server

# Test 7: PRIVMSG to nonexistent user
subheader "Test 7: PRIVMSG to nonexistent user"
start_server
register_user "nothere"
RESPONSE=$(send "PRIVMSG nobody :hi\r\n")
assert_contains "PRIVMSG to nonexistent user returns 401" "$RESPONSE" "401"
kill_server

footer