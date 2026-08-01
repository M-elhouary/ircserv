#!/bin/bash
source "$(dirname "$0")/lib.sh"

header "Basic Connection & Registration Tests"

# --- Test 1: Server accepts connection ---
start_server
RESPONSE=$(send "PASS $PASS\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n")
assert_contains "Server accepts connection" "$RESPONSE" "001"
kill_server

# --- Test 2: Wrong password ---
start_server
RESPONSE=$(send "PASS wrongpass\r\n")
assert_contains "Wrong password -> 464" "$RESPONSE" "464"
kill_server

# --- Test 3: No password given ---
start_server
RESPONSE=$(send "PASS\r\n")
assert_contains "No password param -> 461" "$RESPONSE" "461"
kill_server

# --- Test 4: Already authenticated ---
start_server
RESPONSE=$(send "PASS $PASS\r\nPASS $PASS\r\n")
assert_contains "Double PASS -> 462" "$RESPONSE" "462"
kill_server

# --- Test 5: NICK without PASS ---
start_server
RESPONSE=$(send "NICK testuser\r\n")
assert_contains "NICK without PASS -> 451" "$RESPONSE" "451"
kill_server

# --- Test 6: USER without PASS ---
start_server
RESPONSE=$(send "USER testuser 0 * :Test\r\n")
assert_contains "USER without PASS -> 451" "$RESPONSE" "451"
kill_server

# --- Test 7: Full registration success ---
start_server
RESPONSE=$(send "PASS $PASS\r\nNICK alice\r\nUSER alice 0 * :Alice\r\n")
assert_contains "Full registration -> 001 Welcome" "$RESPONSE" "001"
kill_server

# --- Test 8: Duplicate nickname ---
start_server
# First client registers
send "PASS $PASS\r\nNICK sharednick\r\nUSER user1 0 * :User1\r\n" > /dev/null &
sleep 1
# Second client tries same nickname
RESPONSE=$(send "PASS $PASS\r\nNICK sharednick\r\n")
assert_contains "Duplicate nickname -> 433" "$RESPONSE" "433"
kill_server

# --- Test 9: NICK without params ---
start_server
RESPONSE=$(send "PASS $PASS\r\nNICK\r\n")
assert_contains "NICK no params -> 431" "$RESPONSE" "431"
kill_server

# --- Test 10: USER without params ---
start_server
RESPONSE=$(send "PASS $PASS\r\nNICK testuser\r\nUSER\r\n")
assert_contains "USER no params -> 461" "$RESPONSE" "461"
kill_server

# --- Test 11: Server rejects unknown command ---
start_server
RESPONSE=$(send "PASS $PASS\r\nNICK testuser\r\nUSER testuser 0 * :Test\r\nUNKNOWNCMD\r\n")
assert_contains "Unknown command -> 421" "$RESPONSE" "421"
kill_server

# ============================================
# RESULTS
# ============================================
footer
