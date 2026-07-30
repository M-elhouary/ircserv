#include "ircserver.hpp"
#include <csignal>

volatile sig_atomic_t g_stop_server = 0;

extern "C" void handleShutdownSignal(int) {
    g_stop_server = 1;
}

void Server::init() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // no SA_RESTART: let poll() return EINTR so the loop can exit

    sa.sa_handler = handleShutdownSignal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);

    setupSocket();
    running = true;
}
