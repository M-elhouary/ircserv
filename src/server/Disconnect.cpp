#include "ircserver.hpp"


void Server::disconnectClient(int fd) {
  std::cout << "Client disconnected (fd = " << fd << ")" << std::endl;

  close(fd);

  std::map<int, Client *>::iterator it = clients.find(fd);
  if (it != clients.end()) {
    std::string nick = it->second->getNickName();
    std::map<std::string, Channel *> &channels = getChannels();
    std::vector<std::string> emptyChannels;
    for (std::map<std::string, Channel *>::iterator cit = channels.begin();
         cit != channels.end(); ++cit) {
      cit->second->removeClient(nick);
      cit->second->removeOperator(nick);
      // Check if the channel is empty after removing the client
      // cit->first is the channel name, cit->second is the Channel pointer
      if (cit->second->getMembers().empty()) {
        emptyChannels.push_back(cit->first);
      }
    }
    for(size_t i = 0; i < emptyChannels.size(); ++i) {
      removeChannel(emptyChannels[i]);
    }
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
