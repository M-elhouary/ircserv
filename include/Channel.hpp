#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <string>
#include <vector>

class Channel
{

private:
    std::string _channelName;
    std::vector<Client *> _clients;
    std::vector<Client *> _operators;
    std::vector<Client *> _inviteList; // invited clients
    std::string _topic;                // channel topic
    std::string _password;             // channel key
    int _userLimit;                    // max users (0 = no limit)
    bool _inviteOnly;                  // invite-only mode
    bool _topicRestricted;             // only operators change topic
public:
    Channel(std::string _channelName);
    ~Channel();
    std::string getName() const;
    void addClient(Client *client);
    void addOperator(Client *client);
    void removeClient(Client *client);
    void removeOperator(Client *client);
    std::vector<Client *> &getMembers();
    std::vector<Client *> &getOperators();
    std::vector<Client *> &getInviteList();
    std::string getTopic() const;
    std::string getPassword() const;
    int getUserLimit() const;
    void setTopic(const std::string &topic);
    void setPassword(const std::string &password);
    void setUserLimit(int limit);
    void setInviteOnly(bool value);
    void setTopicRestricted(bool value);
    bool isOperator(Client *client) const;
    bool isClientInChannel(Client *client) const;
    bool isInviteOnly() const;
    bool isTopicRestricted() const;
};

#endif // CHANNEL_HPP