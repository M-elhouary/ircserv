#include "ircserver.hpp"

Channel::Channel(std::string channelName)
{
    _channelName = channelName;
    _topic = "";    // topic mean the subject of the channel
    _password = ""; // channel key
    _userLimit = 0;
    _isClientInvited = false;
    _inviteOnly = false;      // for invite-only channels
    _topicRestricted = false; // for channels where only operators can change the topic
};

Channel::~Channel() {};

// getters
std::string Channel::getName() const
{
    return _channelName;
};
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


std::vector<Client *> &Channel::getClientsChannel()
{
    return _clients;
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

void Channel::addOperator(Client *client)
{
    _operators.push_back(client);
};

void Channel::removeClient(Client *client)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == client)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
};

void Channel::removeOperator(Client *client)
{
    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i] == client)
        {
            _operators.erase(_operators.begin() + i);
            break;
        }
    }
};

bool Channel::isClientInChannel(Client *client) const
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == client)
            return true;
    }
    return false;
};

bool Channel::isOperator(Client *client) const
{

    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i] == client)
            return true;
    }
    return false;
};

bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

bool Channel::isClientInvited(Client *client)
{
    for(size_t i = 0; i < _inviteList.size(); i++)
    {
        if (_inviteList[i] == client)
            return true;
    }   
    return false;
}