#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "INVITE Command Tests"
echo -e "  Syntax: INVITE <nickname> <channel>"
echo ""

register_user()
{
    local nick="$1"
    send "PASS testpass\r\nNICK $nick\r\nUSER $nick 0 * :$nick\r\n" > /dev/null
    sleep 1
}

# Test 1: INVITE user to channel (operator)
subheader "Test 1: Operator invites user to channel"
start_server
register_user "inviter"
send "JOIN #invitechan\r\n" > /dev/null
sleep 1
register_user "invitee"
RESPONSE=$(send "INVITE invitee #invitechan\r\n")
assert_contains "INVITE succeeds" "$RESPONSE" "341"
kill_server

# Test 2: INVITE without registration
subheader "Test 2: INVITE without registration"
start_server
RESPONSE=$(send "INVITE bob #general\r\n")
assert_contains "INVITE without registration returns 451" "$RESPONSE" "451"
kill_server

# Test 3: INVITE no params
subheader "Test 3: INVITE no params"
start_server
register_user "invitenoparam"
RESPONSE=$(send "INVITE\r\n")
assert_contains "INVITE no params returns 461" "$RESPONSE" "461"
kill_server

# Test 4: INVITE nonexistent channel
subheader "Test 4: INVITE to nonexistent channel"
start_server
register_user "inviteghost"
RESPONSE=$(send "INVITE bob #ghost\r\n")
assert_contains "INVITE nonexistent channel returns 403" "$RESPONSE" "403"
kill_server

# Test 5: INVITE user not in channel
subheader "Test 5: INVITE user not in channel"
start_server
register_user "invitenotmember"
RESPONSE=$(send "INVITE bob #general\r\n")
assert_contains "INVITE not in channel returns 442" "$RESPONSE" "442"
kill_server

# Test 6: INVITE nonexistent target
subheader "Test 6: INVITE nonexistent target"
start_server
register_user "invitenotarget"
send "JOIN #invitechan2\r\n" > /dev/null
sleep 1
RESPONSE=$(send "INVITE ghostuser #invitechan2\r\n")
assert_contains "INVITE nonexistent target returns 401" "$RESPONSE" "401"
kill_server

footer