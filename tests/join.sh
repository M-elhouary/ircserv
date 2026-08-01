#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "JOIN Command Tests"
echo -e "  Syntax: JOIN <channel> [<key>]"
echo ""

# Register a user helper
register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: Join new channel (create it)
subheader "Test 1: Join new channel"
start_server
register_user "joiner1"
RESPONSE=$(send "JOIN #testchan\r\n")
assert_contains "JOIN new channel succeeds" "$RESPONSE" "353"
assert_contains "JOIN sends 366 ENDOFNAMES" "$RESPONSE" "366"
kill_server

# Test 2: Join existing channel
subheader "Test 2: Join existing channel"
start_server
register_user "joiner2"
send "JOIN #existing\r\n" > /dev/null
sleep 1
RESPONSE=$(send "PASS testpass\r\nNICK joiner2b\r\nUSER joiner2b 0 * :Joiner2b\r\nJOIN #existing\r\n")
assert_contains "JOIN existing channel succeeds" "$RESPONSE" "353"
kill_server

# Test 3: JOIN without registration
subheader "Test 3: JOIN without registration"
start_server
RESPONSE=$(send "JOIN #general\r\n")
assert_contains "JOIN without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 4: Invalid channel name (no #)
subheader "Test 4: Invalid channel name (no #)"
start_server
register_user "badchan"
RESPONSE=$(send "JOIN general\r\n")
assert_contains "JOIN without # returns error" "$RESPONSE" "403"
kill_server

# Test 5: No params
subheader "Test 5: JOIN with no params"
start_server
register_user "noparam"
RESPONSE=$(send "JOIN\r\n")
assert_contains "JOIN no params returns 461" "$RESPONSE" "461"
kill_server

# Test 6: Channel with password (correct key)
subheader "Test 6: JOIN with correct password"
start_server
register_user "keyuser"
send "JOIN #secret mykey\r\n" > /dev/null
sleep 1
RESPONSE=$(send "PASS testpass\r\nNICK keyuser2\r\nUSER keyuser2 0 * :KeyUser2\r\nJOIN #secret mykey\r\n")
assert_contains "JOIN with correct key succeeds" "$RESPONSE" "353"
kill_server

# Test 7: Channel with password (wrong key)
subheader "Test 7: JOIN with wrong password"
start_server
register_user "wrongkey"
send "JOIN #secret realkey\r\n" > /dev/null
sleep 1
RESPONSE=$(send "PASS testpass\r\nNICK wrongkey2\r\nUSER wrongkey2 0 * :WrongKey\r\nJOIN #secret wrongkey\r\n")
assert_contains "JOIN with wrong key returns 475" "$RESPONSE" "475"
kill_server

footer