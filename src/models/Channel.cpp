#include "ircserver.hpp"

Channel::Channel(std::string channelName)
{
    _channelName = channelName;
    _topic = "";    // topic mean the subject of the channel
    _password = ""; // channel key
    _userLimit = 0;
    _inviteOnly = false;      // for invite-only channels
    _topicRestricted = false; // for channels where only operators can change the topic
};

Channel::~Channel() {};

std::string Channel::getName() const
{
    return _channelName;
};

// getters
std::vector<Client *> &Channel::getMembers()
{
    return _clients;
}

std::vector<Client *> &Channel::getOperators()
{
    return _operators;
}

std::vector<Client *> &Channel::getInviteList()
{
    return _inviteList;
}

std::string Channel::getTopic() const
{
    return _topic;
}

std::string Channel::getPassword() const
{
    return _password;
}
int Channel::getUserLimit() const
{
    return _userLimit;
}

// setters
void Channel::setTopic(const std::string &topic)    
{
    _topic = topic;
}
void Channel::setPassword(const std::string &password)
{
    _password = password;
}
void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
}
void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}
void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}


void Channel::addClient(Client *client)
{
    _clients.push_back(client);
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

void Channel::addOperator(Client *client)
{
    _operators.push_back(client);
};

void Channel::removeOperator(Client *client)
{
    (void)client;
};

bool Channel::isOperator(Client *client) const
{
    (void)client;
    return false;
};
