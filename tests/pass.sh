#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "PASS Command Tests"
echo -e "  Syntax: PASS <password>"
echo ""

# Test 1: Correct password
subheader "Test 1: Correct password"
start_server
RESPONSE=$(send "PASS testpass\r\n")
assert_not_contains "No 462 error on correct password" "$RESPONSE" "462"
assert_not_contains "No 464 error on correct password" "$RESPONSE" "464"
kill_server

# Test 2: Wrong password
subheader "Test 2: Wrong password"
start_server
RESPONSE=$(send "PASS wrongpass\r\n")
assert_contains "Wrong password returns 464" "$RESPONSE" "464"
kill_server

# Test 3: No password parameter
subheader "Test 3: No password parameter"
start_server
RESPONSE=$(send "PASS\r\n")
assert_contains "No params returns 461" "$RESPONSE" "461"
kill_server

# Test 4: Double PASS (already authenticated) - same connection
subheader "Test 4: Double PASS (already authenticated)"
start_server
RESPONSE=$(send "PASS testpass\r\nPASS testpass\r\n")
assert_contains "Double PASS returns 462" "$RESPONSE" "462"
kill_server

footer