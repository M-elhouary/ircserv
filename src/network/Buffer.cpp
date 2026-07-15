#include "ircserver.hpp"

void Server::processClientBuffer(Client *client) {
  std::string &buffer = client->getRecvBufferRef();

  size_t pos;
  while ((pos = buffer.find("\r\n")) != std::string::npos) {
    std::string line = buffer.substr(0, pos);
    client->consumeFromRecvBuffer(pos + 2);

    if (line.empty())
      continue;

    std::cout << "[DBUG]:[fd=" << client->getFd() << "] " << line << std::endl;
    dispatch(*client, line, *this);
  }
}
