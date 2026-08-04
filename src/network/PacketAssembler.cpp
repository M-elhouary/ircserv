#include "ircserver.hpp"

void Server::processClientBuffer(Client *client) {
  std::string &buffer = client->getRecvBufferRef();

  size_t pos;

  while ((pos = buffer.find('\n')) != std::string::npos) {
    std::string line = buffer.substr(0, pos);

    client->consumeFromRecvBuffer(pos + 1);

    if (!line.empty() && line[line.size() - 1] == '\r')
      line.erase(line.size() - 1);

    if (line.empty())
      continue;

    if (line.size() + 2 > 512) {
      std::cerr << "Client fd=" << client->getFd()
                << " sent message exceeding 512 bytes"
                << std::endl;

      disconnectClient(client->getFd());
      return;
    }
    std::cout << "Received from fd=" << client->getFd() << ": " << line
              << std::endl;
    dispatch(*client, line, *this);
  }
}
