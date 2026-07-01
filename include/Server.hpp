#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <poll.h>

class Server {
  public:
    Server(int port, const std::string& password);
    ~Server();

    void init();
    void run();
    void stop();

  private:
    int port;
    std::string password;
    int server_sock;
    bool running;
    std::vector<struct pollfd> pfds;

    void setupSocket();
    void cleanup();
};

#endif
