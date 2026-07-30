#include "ircserver.hpp"

// src/commands/Mode.cpp
// └── void handleMode(Client& client, IRCMessage& msg, Server& server)
//     ├── check: isRegistred()              → 451
//     ├── find channel                      → 403
//     ├── client is operator?               → 482
//     ├── parse mode string (+i -i +t -t +k -k +o -o +l -l)
//     └── apply each mode change

void handleCurrentMode(Client &client, Channel *Channel)
{
    std::string CurrentMode = "";
    int num = Channel->getUserLimit();
    std::string UserLimit;

    if (Channel->isInviteOnly())
        CurrentMode.push_back('i');
    if (Channel->isTopicRestricted())
        CurrentMode.push_back('t');
    if (!Channel->getPassword().empty())
        CurrentMode.push_back('k');
    if (Channel->getUserLimit() > 0)
    {
        std::ostringstream oss;
        oss << num;
        UserLimit = oss.str();
        CurrentMode.push_back('l');
    }
    if (Channel->getOperators().size() > 0)
        CurrentMode.push_back('o');

    if (CurrentMode.find('k') != std::string::npos && CurrentMode.find('l') != std::string::npos)
    {
        client.sendMessage(":ircserv * 324 " + client.getNickName() + " " + Channel->getName() + " +" + CurrentMode + Channel->getPassword() + " " + UserLimit + "\r\n");
        return;
    }
    else if (CurrentMode.find('k') != std::string::npos)
    {
        client.sendMessage(":ircserv * 324 " + client.getNickName() + " " + Channel->getName() + " +" + CurrentMode + " " + Channel->getPassword() + "\r\n");
        return;
    }
    else if (CurrentMode.find('l') != std::string::npos)
    {
        client.sendMessage(":ircserv * 324 " + client.getNickName() + " " + Channel->getName() + " +" + CurrentMode + " " + UserLimit + "\r\n");
        return;
    }

    if (!CurrentMode.empty())
        client.sendMessage(":ircserv * 324 " + client.getNickName() + " " + Channel->getName() + " +" + CurrentMode + "\r\n");
    else
        client.sendMessage(":ircserv * 324 " + client.getNickName() + " " + Channel->getName() + " :No modes are set\r\n");

    return;
}

void AddMode(Channel *Channel, Client &client, IRCMessage &msg, Server &server)
{

    for (size_t i = 1; i < msg.params[1].length(); i++)
    {
        if (msg.params[1][i] == 'i')
        {
            Channel->setInviteOnly(true);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " +i\r\n", &client);
        }
        else if (msg.params[1][i] == 't')
        {
            Channel->setTopicRestricted(true);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " +t\r\n", &client);

        }
        else if (msg.params[1][i] == 'k')
        {
            if (msg.params.size() < 3)
            {
                client.sendMessage(":ircserv 461 * :Not enough parameters for +k\r\n");
                return;
            }
            Channel->setPassword(msg.params[2]);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " +k\r\n", &client);
        }
        else if (msg.params[1][i] == 'l')
        {
            if (msg.params.size() < 3)
            {
                client.sendMessage(":ircserv 461 * :Not enough parameters for +l\r\n");
                return;
            }
            std::stringstream ss(msg.params[2]);
            int limit = 0;
            ss >> limit;
            Channel->setUserLimit(limit);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " +l\r\n", &client);    
        }
        else if (msg.params[1][i] == 'o')
        {
            if (msg.params.size() < 3)
            {
                client.sendMessage(":ircserv 461 * :Not enough parameters for +o\r\n");
                return;
            }
            std::map<int, Client *> &Clients = server.getClients();
            for (std::map<int, Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
            {
                if (it->second->getNickName() == msg.params[2])
                {
                    Channel->addOperator(it->second);
                    Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " +o " + msg.params[2] + "\r\n", &client);
                    return;
                }
            }
            client.sendMessage(":ircserv 401 * :No such nick/channel\r\n");
            return;
        }
        else
        {
client.sendMessage(":ircserv 501 * :Unknown MODE flag\r\n");
                return;
        }
    }
    

}

void DesactiveMode(Channel *Channel, Client &client, IRCMessage &msg)
{

    for (size_t i = 1; i < msg.params[1].length(); i++)
    {
        if (msg.params[1][i] == 'i')
        {
            Channel->setInviteOnly(false);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " -i\r\n", &client);
        }
        else if (msg.params[1][i] == 't')
        {
            Channel->setTopicRestricted(false);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " -t\r\n", &client);    

        }
        else if (msg.params[1][i] == 'k')
        {
            if (msg.params.size() < 3)
            {
                client.sendMessage(":ircserv 461 * :Not enough parameters for -k\r\n");
                return;
            }
            if (Channel->getPassword() != msg.params[2])
            {
                client.sendMessage(":ircserv 475 * :Password incorrect\r\n");
                return;
            }
            Channel->setPassword("");
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " -k\r\n", &client);
        }
        else if (msg.params[1][i] == 'l')
        {
            Channel->setUserLimit(0);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " -l\r\n", &client);    

        }
        else if (msg.params[1][i] == 'o')
        {
            if (msg.params.size() < 3)
            {
                client.sendMessage(":ircserv 461 * :Not enough parameters for -o\r\n");
                return;
            }
            Channel->removeOperator(msg.params[2]);
            Channel->broadcast(":" + client.getNickName() + " MODE " + Channel->getName() + " -o " + msg.params[2] + "\r\n", &client);  

        }
        else
        {
client.sendMessage(":ircserv 501 * :Unknown MODE flag\r\n");
                return;
        }
    }
    
}

void handleMode(Client &client, IRCMessage &msg, Server &server)
{
    if (!client.isRegistred())
    {
        client.sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty())
    {
        client.sendMessage(":ircserv 461 * :Not enough parameters\r\n");
        return;
    }

    Channel *Channel = server.getChannel(msg.params[0]);
    if (Channel == NULL)
    {
        client.sendMessage(":ircserv 403 * :No such channel\r\n");
        return;
    }

    if (!Channel->isClientInChannel(&client))
    {
        client.sendMessage(":ircserv 442 * :You're not on that channel\r\n");
        return;
    }

    if (msg.params.size() == 1)
    {
        handleCurrentMode(client, Channel);
        return;
    }

    if (msg.params.size() >= 2)
    {
        if (!Channel->isOperator(client.getNickName()))
        {
            client.sendMessage(":ircserv 482 * :You're not channel operator\r\n");
            return;
        }
        if (msg.params[1][0] != '+' && msg.params[1][0] != '-')
        {
client.sendMessage(":ircserv 501 * :Unknown MODE flag\r\n");
                return;
        }

        if (msg.params[1][0] == '+')
            AddMode(Channel, client, msg, server);
        else if (msg.params[1][0] == '-')
            DesactiveMode(Channel, client, msg);
    }
}