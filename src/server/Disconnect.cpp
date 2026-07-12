#include "Server.hpp"
#include <iostream>
#include <unistd.h>

void Server::disconnectClient(int fd) {
  std::cout << "Client disconnected (fd = " << fd << ")" << std::endl;

  close(fd);

  std::map<int, Client *>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    delete it->second;
    clients.erase(it);
  }

  for (size_t i = 0; i < pfds.size(); i++) {
    if (pfds[i].fd == fd) {
      pfds.erase(pfds.begin() + i);
      break;
    }
  }
}
