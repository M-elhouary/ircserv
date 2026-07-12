#ifndef SERVER_HPP
#define SERVER_HPP

#include <cerrno>
#include <map>
#include <poll.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "Client.hpp"

class Server {
public:
  Server(int port, const std::string &password);
  ~Server();

  void init();
  void run();
  const std::string &getPassword() const;

private:
  int port;
  std::string password;
  int server_sock;
  bool running;
  std::vector<struct pollfd> pfds;
  std::map<int, Client *> clients;

  void acceptClient(int server_fd);
  void disconnectClient(int fd);

  bool handleClientData(int fd);
  void processClientBuffer(Client *client);
  void setupSocket();
  void cleanup();
};

#endif
