#include "ircserver.hpp"

void Server::setupSocket() {
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    std::cerr << "Error: socket() failed" << std::endl;
    exit(1);
  }

  int optval = 1;
  if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) < 0) {
    std::cerr << "Error: setsockopt() failed" << std::endl;
    close(server_sock);
    exit(1);
  }

  int flags = fcntl(server_sock, F_GETFL, 0);
  if (flags < 0 || fcntl(server_sock, F_SETFL, flags | O_NONBLOCK) < 0) {
    std::cerr << "Error: fcntl() failed" << std::endl;
    close(server_sock);
    exit(1);
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(server_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
      0) {
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
    int ret = poll(&pfds[0], pfds.size(), -1);

    if (ret < 0) {
      if (errno == EINTR)
        continue;
      std::cerr << "Error: poll() failed" << std::endl;
      break;
    }

    for (size_t i = 0; i < pfds.size(); i++) {
      if (pfds[i].revents == 0)
        continue;

      if (pfds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
        if (pfds[i].fd != server_sock) {
          disconnectClient(pfds[i].fd);
          i--;
        }
        continue;
      }

      if (pfds[i].revents & POLLIN) {
        if (pfds[i].fd == server_sock) {
          acceptClient(pfds[i].fd);
        } else {
          bool disconnected = handleClientData(pfds[i].fd);
          if (disconnected) {
            i--;
            continue;
          }
          std::map<int, Client *>::iterator sit = clients.find(pfds[i].fd);
          if (sit != clients.end() && sit->second->hasPendingSend())
            pfds[i].events |= POLLOUT;
        }
      }

      if (pfds[i].revents & POLLOUT) {
        std::map<int, Client *>::iterator it = clients.find(pfds[i].fd);
        if (it != clients.end()) {
          int send_ret = it->second->flushSendBuffer();
          if (send_ret <= 0) {
            disconnectClient(pfds[i].fd);
            i--;
            continue;
          }
          if (!it->second->hasPendingSend())
            pfds[i].events &= ~POLLOUT;
        }
      }
    }
  }

  cleanup();
}
