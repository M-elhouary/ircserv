#!/bin/bash

# ============================================
# Shared Test Helper Library
# ============================================

PORT=6670
PASSWORD="testpass"
SERVER_PID=""
PASS_COUNT=0
FAIL_COUNT=0
TOTAL=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

start_server()
{
    ./ircserv $PORT $PASSWORD > /dev/null 2>&1 &
    SERVER_PID=$!
    sleep 1
}

kill_server()
{
    if [ -n "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
        SERVER_PID=""
    fi
}

send()
{
    echo -e "$1" | nc -w 3 localhost $PORT 2>/dev/null
}

assert_contains()
{
    local test_name="$1"
    local response="$2"
    local expected="$3"

    TOTAL=$((TOTAL + 1))
    if echo "$response" | grep -q "$expected"; then
        echo -e "${GREEN}  PASS${NC}: $test_name"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "${RED}  FAIL${NC}: $test_name (expected: '$expected')"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
}

assert_not_contains()
{
    local test_name="$1"
    local response="$2"
    local unexpected="$3"

    TOTAL=$((TOTAL + 1))
    if echo "$response" | grep -q "$unexpected"; then
        echo -e "${RED}  FAIL${NC}: $test_name (should not contain: '$unexpected')"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    else
        echo -e "${GREEN}  PASS${NC}: $test_name"
        PASS_COUNT=$((PASS_COUNT + 1))
    fi
}

header()
{
    echo ""
    echo -e "${BLUE}═══════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════${NC}"
}

subheader()
{
    echo -e "${CYAN}── $1 ──${NC}"
}

footer()
{
    echo ""
    echo -e "${BLUE}═══════════════════════════════════════════${NC}"
    if [ $FAIL_COUNT -eq 0 ]; then
        echo -e "${GREEN}  ALL TESTS PASSED: $PASS_COUNT/$TOTAL${NC}"
    else
        echo -e "${RED}  RESULTS: $PASS_COUNT/$TOTAL passed, $FAIL_COUNT failed${NC}"
    fi
    echo -e "${BLUE}═══════════════════════════════════════════${NC}"
    echo ""
}

cleanup()
{
    kill_server
}

trap cleanup EXIT
