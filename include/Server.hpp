#ifndef SERVER_HPP
#define SERVER_HPP

#include <cerrno>
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

  void acceptClient(int server_fd);
  void disconnectClient(int fd);
  void setupSocket();
  void cleanup();
};

#endif
