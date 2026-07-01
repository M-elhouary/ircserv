#!/bin/bash

./ircserv 6667 password &
SERVER_PID=$!

sleep 2

echo "PASS password" | nc localhost 6667
echo "NICK user1" | nc localhost 6667
echo "USER user1 0 * :user1" | nc localhost 6667

kill $SERVER_PID