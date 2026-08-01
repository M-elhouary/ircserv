#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "USER Command Tests"
echo -e "  Syntax: USER <username> <hostname> <servername> :<realname>"
echo ""

# Test 1: Valid USER after PASS
subheader "Test 1: Valid USER after PASS"
start_server
RESPONSE=$(send "PASS testpass\r\nUSER testuser 0 * :Test User\r\n")
assert_contains "Valid USER accepted" "$RESPONSE" "451"
kill_server

# Test 2: USER without PASS
subheader "Test 2: USER without PASS"
start_server
RESPONSE=$(send "USER testuser 0 * :Test User\r\n")
assert_contains "USER without PASS returns 451" "$RESPONSE" "451"
kill_server

# Test 3: No params
subheader "Test 3: No params"
start_server
RESPONSE=$(send "PASS testpass\r\nUSER\r\n")
assert_contains "USER no params returns 461" "$RESPONSE" "461"
kill_server

# Test 4: USER with only 1 param
subheader "Test 4: USER with only 1 param"
start_server
RESPONSE=$(send "PASS testpass\r\nUSER testuser\r\n")
assert_contains "USER missing params returns 461" "$RESPONSE" "461"
kill_server

# Test 5: Full registration flow
subheader "Test 5: Full registration (PASS + NICK + USER)"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK fulluser\r\nUSER fulluser 0 * :Full User\r\n")
assert_contains "Full registration returns 001 Welcome" "$RESPONSE" "001"
kill_server

# Test 6: Double USER - same connection
subheader "Test 6: Double USER"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK dupuser\r\nUSER dupuser 0 * :Dup User\r\nUSER dupuser 0 * :Dup User\r\n")
assert_contains "Double USER returns 462" "$RESPONSE" "462"
kill_server

footer