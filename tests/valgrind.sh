#!/bin/bash

timeout 10 \
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-fds=yes \
    --error-exitcode=1 \
    ./ircserv 6667 password