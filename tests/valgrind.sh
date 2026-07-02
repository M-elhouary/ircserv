#!/bin/bash

timeout 5 \
valgrind \
    --leak-check=full \
    --show-leak-kinds=definite,indirect \
    --track-fds=yes \
    --error-exitcode=1 \
    ./ircserv 6667 password
