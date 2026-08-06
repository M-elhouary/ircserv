#include "ircserver.hpp"

void Server::acceptClient(int server_fd) {
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr);
  
  int client_fd =
  accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &addrlen);

  if (client_fd < 0)
        return;

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

  clients[client_fd] = new Client(client_fd);
  clients[client_fd]->setHostname(client_addr);

  std::cout << "New client connected (fd = " << client_fd << ")" << std::endl;
}
