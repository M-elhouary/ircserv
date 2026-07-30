#include "ircserver.hpp"

void Server::cleanup() {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    close(it->first);
    delete it->second;
  }
  clients.clear();

  for (std::map<std::string, Channel *>::iterator it = channels.begin();
       it != channels.end(); ++it)
    delete it->second;
  channels.clear();

  if (server_sock >= 0) {
    close(server_sock);
    server_sock = -1;
  }
  pfds.clear();
}
