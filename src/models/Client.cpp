#include "ircserver.hpp"

Client::Client(int fd)
 : _fd(fd), _nickname(""), 
 _username(""), 
 _authenticated(false),
  _nicknameReceived(false), 
  _registred(false),
   _welcomeSent(false) 
{
    
}

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

bool Client::getWelcomeSent() const
{
    return this->_welcomeSent;
}

void Client::setWelcomeSent(bool value)
{
    this->_welcomeSent = value;
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
    (void) messgae;
}
Client::~Client() {}

const std::string &Client::getRecvBuffer() const { return recv_buffer; }

std::string &Client::getRecvBufferRef() { return recv_buffer; }

void Client::appendToRecvBuffer(const std::string &data) { recv_buffer += data; }

void Client::consumeFromRecvBuffer(size_t n) { recv_buffer.erase(0, n); }
