#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "NICK Command Tests"
echo -e "  Syntax: NICK <nickname>"
echo ""

# Test 1: Valid nickname after PASS
subheader "Test 1: Valid nickname after PASS"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK alice\r\n")
assert_contains "Valid NICK accepted" "$RESPONSE" "001"
kill_server

# Test 2: NICK without PASS
subheader "Test 2: NICK without PASS"
start_server
RESPONSE=$(send "NICK alice\r\n")
assert_contains "NICK without PASS returns 451" "$RESPONSE" "451"
kill_server

# Test 3: No nickname parameter
subheader "Test 3: No nickname parameter"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK\r\n")
assert_contains "NICK no params returns 431" "$RESPONSE" "431"
kill_server

# Test 4: Invalid nickname (starts with digit)
subheader "Test 4: Invalid nickname (starts with digit)"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK 1abc\r\n")
assert_contains "Nickname starting with digit returns 432" "$RESPONSE" "432"
kill_server

# Test 5: Invalid nickname (contains space)
subheader "Test 5: Invalid nickname (contains space)"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK a b\r\n")
assert_contains "Nickname with space returns 432" "$RESPONSE" "432"
kill_server

# Test 6: Duplicate nickname - same connection
subheader "Test 6: Duplicate nickname"
start_server
RESPONSE=$(send "PASS testpass\r\nNICK bob\r\nUSER bob 0 * :Bob\r\nNICK bob\r\n")
assert_contains "Duplicate nickname returns 433" "$RESPONSE" "433"
kill_server

footer