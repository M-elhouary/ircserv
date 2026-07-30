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

    dispatch(*client, line, *this);
  }
}
