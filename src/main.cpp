#include "ircserver.hpp"

int main(int ac, char **av) 
{
  if (ac != 3) {
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    return 1;
  }

  int port = std::atoi(av[1]);
  if (port <= 1025 || port > 65535) {
    std::cerr << "Error: Invalid port number" << std::endl;
    return 1;
  }

  std::string password = av[2];
  if (password.empty()) {
    std::cerr << "Error: Password cannot be empty" << std::endl;
    return 1;
  }

  Server server(port, password);
  server.init();
  server.run();

  return 0;
}
