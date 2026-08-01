#include "ircserver.hpp"

void Server::run() {
  while (running && !g_stop_server) {
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

      if (pfds[i].revents & (POLLHUP | POLLERR)) {
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
          it->second->flushSendBuffer();
          if (!it->second->hasPendingSend())
            pfds[i].events &= ~POLLOUT;
        }
      }
    }

    // Arm POLLOUT for every client with buffered output (e.g. broadcast
    // recipients), otherwise their queued data would never be flushed.
    for (size_t j = 0; j < pfds.size(); j++) {
      if (pfds[j].fd == server_sock)
        continue;
      std::map<int, Client *>::iterator cit = clients.find(pfds[j].fd);
      if (cit != clients.end() && cit->second->hasPendingSend())
        pfds[j].events |= POLLOUT;
    }
  }

  cleanup();
}
