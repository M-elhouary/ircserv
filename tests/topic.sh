#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "TOPIC Command Tests"
echo -e "  Syntax: TOPIC <channel> [:<new topic>]"
echo ""

register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: View topic (no topic set)
subheader "Test 1: View topic when no topic set"
start_server
register_user "topicviewer1"
send "JOIN #topictest\r\n" > /dev/null
sleep 1
RESPONSE=$(send "TOPIC #topictest\r\n")
assert_contains "View empty topic returns 331" "$RESPONSE" "331"
kill_server

# Test 2: View topic (topic set)
subheader "Test 2: View topic when topic is set"
start_server
register_user "topicviewer2"
send "JOIN #topictest2\r\n" > /dev/null
sleep 1
send "TOPIC #topictest2 :Welcome!\r\n" > /dev/null
sleep 1
RESPONSE=$(send "TOPIC #topictest2\r\n")
assert_contains "View set topic returns 332" "$RESPONSE" "332"
kill_server

# Test 3: Set topic (operator)
subheader "Test 3: Set topic as operator"
start_server
register_user "topicsetter"
send "JOIN #topictest3\r\n" > /dev/null
sleep 1
RESPONSE=$(send "TOPIC #topictest3 :New Topic Here\r\n")
assert_contains "Set topic succeeds" "$RESPONSE" "TOPIC"
kill_server

# Test 4: TOPIC without registration
subheader "Test 4: TOPIC without registration"
start_server
RESPONSE=$(send "TOPIC #general\r\n")
assert_contains "TOPIC without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 5: TOPIC nonexistent channel
subheader "Test 5: TOPIC nonexistent channel"
start_server
register_user "topicitem"
RESPONSE=$(send "TOPIC #ghost\r\n")
assert_contains "TOPIC nonexistent channel returns 403" "$RESPONSE" "403"
kill_server

# Test 6: TOPIC user not in channel
subheader "Test 6: TOPIC user not in channel"
start_server
register_user "topicoutsider"
RESPONSE=$(send "TOPIC #general :new topic\r\n")
assert_contains "TOPIC not in channel returns 442" "$RESPONSE" "442"
kill_server

footer