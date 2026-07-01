#include <arpa/inet.h>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>

namespace
{
volatile sig_atomic_t g_running = 1;

void handleSignal(int)
{
    g_running = 0;
}

bool setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}
}

int main()
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "socket: " << std::strerror(errno) << '\n';
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsockopt: " << std::strerror(errno) << '\n';
        close(server_fd);
        return 1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cerr << "bind: " << std::strerror(errno) << '\n';
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) < 0)
    {
        std::cerr << "listen: " << std::strerror(errno) << '\n';
        close(server_fd);
        return 1;
    }

    if (!setNonBlocking(server_fd))
    {
        std::cerr << "fcntl: " << std::strerror(errno) << '\n';
        close(server_fd);
        return 1;
    }

    std::vector<pollfd> fds;
    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    fds.push_back(server_pollfd);

    std::cout << "Small test server listening on port 6667\n";

    while (g_running)
    {
        int ready = poll(&fds[0], fds.size(), 1000);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            std::cerr << "poll: " << std::strerror(errno) << '\n';
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == server_fd)
            {
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
                if (client_fd < 0)
                {
                    if (errno != EWOULDBLOCK && errno != EAGAIN)
                        std::cerr << "accept: " << std::strerror(errno) << '\n';
                    continue;
                }

                setNonBlocking(client_fd);

                pollfd client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;
                client_pollfd.revents = 0;
                fds.push_back(client_pollfd);

                char ip[INET_ADDRSTRLEN];
                const char* addr_str = inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
                std::cout << "Client connected from " << (addr_str ? addr_str : "unknown")
                          << ':' << ntohs(client_addr.sin_port) << '\n';
            }
            else
            {
                char buffer[1024];
                ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0)
                {
                    if (bytes < 0)
                        std::cerr << "recv: " << std::strerror(errno) << '\n';
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i;
                    continue;
                }

                buffer[bytes] = '\0';
                std::cout << "Client " << fds[i].fd << ": " << buffer;
                send(fds[i].fd, buffer, bytes, 0);
            }
        }

        for (size_t i = 0; i < fds.size(); ++i)
            fds[i].revents = 0;
    }

    for (size_t i = 0; i < fds.size(); ++i)
        close(fds[i].fd);

    return 0;
}