#include "ircserver.hpp"

const std::string &Server::getPassword() const
{
    return this->password;
}

std::map<int, Client *> &Server::getClients() { return clients; }
