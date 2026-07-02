#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void Server::acceptClient(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int client_fd = accept(server_fd,
                           reinterpret_cast<sockaddr*>(&client_addr),
                           &addrlen);

    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            std::cerr << "Error: accept() failed" << std::endl;
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error: fcntl() on client failed" << std::endl;
        close(client_fd);
        return;
    }

    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);

    std::cout << "New client connected (fd = " << client_fd << ")" << std::endl;
}
