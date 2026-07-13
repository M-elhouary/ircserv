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
    (void)client;
};

void Channel::removeClient(Client *client)
{
    (void)client;
};


bool Channel::isClientInChannel(Client *client) const
{
    (void)client;
    return false;
};

void Channel::addOperator(const std::string &nickname)
{
    (void)nickname;
};

void Channel::removeOperator(const std::string &nickname)
{
    (void)nickname;
};


bool Channel::isOperator(const std::string &nickname) const
{
    (void)nickname;
    return false;   
};