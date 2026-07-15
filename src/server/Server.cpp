#include "ircserver.hpp"

const std::string &Server::getPassword() const
{
    return this->password;
}

std::map<int, Client *> &Server::getClients() { return clients; }

std::map<std::string, Channel *> &Server::getChannels() { return channels; }
