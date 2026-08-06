#include "ircserver.hpp"

volatile sig_atomic_t g_stop_server = 0;

void handleShutdownSignal(int) {
    g_stop_server = 1;
}

void Server::init() {
    signal(SIGINT, handleShutdownSignal);
    signal(SIGTERM, handleShutdownSignal);
    signal(SIGQUIT, handleShutdownSignal);
    signal(SIGPIPE, SIG_IGN);

    setupSocket();
    running = true;
}
