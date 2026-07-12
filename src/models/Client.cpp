#include "ircserver.hpp"

Client::Client(int fd) : _fd(fd), _nickname(""), _username(""), _authenticated(false), _nicknameReceived(false), _registred(false) {}

int Client::getFd() const
{
    return this->_fd;
}

std::string Client::getNickName()
{
    return this->_nickname;
}

std::string Client::getUserName()
{
    return this->_username;
}


bool Client::getAutenticated()
{
    return this->_authenticated;
}

void Client::setNickName(const std::string nickname)
{
    this->_nickname = nickname;
}

void Client::setUserName(const std::string username)
{
    this->_username = username;
}
void Client::setAutenticated(bool _authenticate)
{
    this->_authenticated = _authenticate;
}
void Client::setNickNameReceived(bool _nicknameReceived)
{
    this->_nicknameReceived = _nicknameReceived;
}

void Client::setRegistred(bool _registred)
{
    this->_registred = _registred;
}

bool Client::isRegistred() const
{
    return (this->_nicknameReceived && this->_authenticated && this->_registred);
}



void Client::sendMessage(std::string messgae)
{

}
Client::~Client() {}