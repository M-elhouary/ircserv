#include "ircserver.hpp"

Client::Client(int fd)
 : _fd(fd), _nickname(""), 
 _username(""), 
 _authenticated(false),
  _nicknameReceived(false), 
  _registred(false),
   _welcomeSent(false),
   _passwordReceived(false)
{
    
}



// Destructor for the Client class. Cleans up any resources associated with the client.
int Client::getFd() const
{
    return this->_fd;
}

// Returns a flag for check if the client has sent a nickname
std::string Client::getNickName()
{
    return this->_nickname;
}


// flag for check if the client has sent a username
std::string Client::getUserName()
{
    return this->_username;
}

// flag for check if the client has sent a password 
// 
bool Client::getAutenticated()
{
    return this->_authenticated;
}

bool Client::getWelcomeSent() const
{
    return this->_welcomeSent;
}

std::string Client::getHostname() const
{
    return this->_hostname;
}

bool Client::getNickNameReceived() const
{
    return this->_nicknameReceived;
}

bool Client::getPasswordReceived() const
{
    return this->_passwordReceived;
}



void Client::setPasswordReceived(bool value)
{
    this->_passwordReceived = value;
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

void Client::setHostname(struct sockaddr_in client_addr) {
  _hostname = inet_ntoa(client_addr.sin_addr);
}

bool Client::isRegistred() const
{
    return (this->_nicknameReceived && this->_authenticated && this->_registred);
}



void Client::sendMessage(const std::string &message)
{
    if (message.empty())
        return;
    sendBuffer += message;
}

bool Client::hasPendingSend() const
{
    return !sendBuffer.empty();
}

Client::~Client() {}
