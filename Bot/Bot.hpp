#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <vector>
#include "QuoteManager.hpp"

class Bot {
  private:
    int            socketFd;
    int            port;
    std::string    host;
    std::string    password;
    std::string    nickname;
    std::string    recvBuffer;
    QuoteManager   quoteManager;

    void sendCommand(const std::string& command);
    void sendQuote(const std::string& target);
    void processLine(const std::string& line);
    void handleMessage(const std::string& prefix,
                       const std::string& command,
                       const std::vector<std::string>& params);
    Bot(const Bot&);
    Bot& operator=(const Bot&);

  public:
    Bot(const std::string& host, int port, const std::string& password);

    ~Bot();

    void connectToServer();
    void login(const std::string& nickname, const std::string& realname);
    void joinChannel(const std::string& channel);
    void run();
};

#endif
