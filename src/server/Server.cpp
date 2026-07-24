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

Channel *Server::getChannel(const std::string &name) {
  std::map<std::string, Channel *>::iterator it = channels.find(name);
  if (it != channels.end())
    return it->second;
  return NULL;
}

Channel *Server::createChannel(const std::string &name, Client *creator) {
  Channel *ch = new Channel(name);
  ch->addClient(creator);
  ch->addOperator(creator);
  channels[name] = ch;
  return ch;
}

void Server::removeChannel(const std::string &name) {
  std::map<std::string, Channel *>::iterator it = channels.find(name);
  if (it != channels.end()) {
    delete it->second;
    channels.erase(it);
  }
}

std::map<std::string, Channel *> &Server::getChannels() {
  return channels;
}

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
