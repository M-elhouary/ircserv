#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

Server::Server(int port, const std::string& password) {
    this->port = port;
    this->password = password;
    server_sock = -1;
    running = false;
}

Server::~Server() {
    cleanup();
}

void Server::setupSocket() {
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        std::cerr << "Error: socket() failed" << std::endl;
        exit(1);
    }

    int optval = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) < 0) {
        std::cerr << "Error: setsockopt() failed" << std::endl;
        close(server_sock);
        exit(1);
    }

    if (fcntl(server_sock, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error: fcntl() failed" << std::endl;
        close(server_sock);
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: bind() to port " << port << " failed" << std::endl;
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 128) < 0) {
        std::cerr << "Error: listen() failed" << std::endl;
        close(server_sock);
        exit(1);
    }

    struct pollfd pfd;
    pfd.fd = server_sock;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);

    std::cout << "Server listening on port " << port << std::endl;
}

void Server::init() {
    setupSocket();
    running = true;
}

void Server::run() {
    while (running) {
      //TODO: poll() loop
    }
}

void Server::stop() {
    running = false;
}

void Server::cleanup() {
    if (server_sock >= 0) {
        close(server_sock);
        server_sock = -1;
    }
    pfds.clear();
}
