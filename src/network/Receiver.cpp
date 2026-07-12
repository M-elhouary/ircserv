#include "Server.hpp"
#include <cerrno>
#include <iostream>
#include <sys/socket.h>

static const size_t MAX_RECV_BUFFER = 8192;

bool Server::handleClientData(int fd) {
  std::map<int, Client *>::iterator it = clients.find(fd);
  if (it == clients.end())
    return true;

  Client *client = it->second;

  char buf[1024];
  int bytes = recv(fd, buf, sizeof(buf) - 1, 0);

  if (bytes <= 0) {
    if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
      return false;
    disconnectClient(fd);
    return true;
  }

  buf[bytes] = '\0';
  client->appendToRecvBuffer(std::string(buf, static_cast<size_t>(bytes)));

  std::cout << "[fd=" << fd << "] buffer: " << client->getRecvBuffer() << std::endl;

  if (client->getRecvBuffer().size() > MAX_RECV_BUFFER) {
    std::cerr << "Client fd=" << fd
              << " exceeded max receive buffer, disconnecting" << std::endl;
    disconnectClient(fd);
    return true;
  }

  processClientBuffer(client);
  return false;
}
