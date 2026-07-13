#include "ircserver.hpp"

Channel::Channel(std::string name)
{};


Channel::~Channel()
{};

std::string Channel::getName() const
{
    return _channelName;
};

void Channel::addClient(Client *client)
{

};

void Channel::removeClient(Client *client)
{

};

bool Channel::isClientInChannel(Client *client) const
{

};

void Channel::addOperator(const std::string &nickname)
{

};

void Channel::removeOperator(const std::string &nickname)
{

};

bool Channel::isOperator(const std::string &nickname) const
{
    
};