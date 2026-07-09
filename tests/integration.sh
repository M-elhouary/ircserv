#!/bin/bash

echo "=== Integration Tests ==="

# Start server in background
./ircserv 6667 password &
SERVER_PID=$!
sleep 2

# Test: server accepts connection
echo "Testing: server accepts connections..."
echo -e "PASS password\r\nNICK user1\r\nUSER user1 0 * :user1\r\n" | nc -q 1 localhost 6667

if [ $? -eq 0 ]; then
    echo "PASS: connection test"
else
    echo "FAIL: connection test"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

# Test: multiple clients
echo "Testing: multiple clients..."
echo -e "PASS password\r\nNICK user2\r\nUSER user2 0 * :user2\r\n" | nc -q 1 localhost 6667 &
echo -e "PASS password\r\nNICK user3\r\nUSER user3 0 * :user3\r\n" | nc -q 1 localhost 6667 &
sleep 2

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "=== All integration tests passed ==="
