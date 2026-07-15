#include "ircserver.hpp"

Server::Server(int port, const std::string &password) {
  this->port = port;
  this->password = password;
  server_sock = -1;
  running = false;
}

Server::Server(const Server &other)
    : port(other.port), password(other.password), server_sock(-1),
      running(false) {}

Server &Server::operator=(const Server &other) {
  if (this != &other) {
    port = other.port;
    password = other.password;
  }
  return *this;
}

Server::~Server() { cleanup(); }

const std::string &Server::getPassword() const { return password; }

std::map<int, Client *> &Server::getClients() { return clients; }

void Server::cleanup() {
  for (std::map<int, Client *>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    close(it->first);
    delete it->second;
  }
  clients.clear();

  if (server_sock >= 0) {
    close(server_sock);
    server_sock = -1;
  }
  pfds.clear();
}
