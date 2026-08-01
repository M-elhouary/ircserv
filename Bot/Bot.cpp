#include "Bot.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Bot::Bot(const std::string& host, int port, const std::string& password)
    : socketFd(-1), port(port), host(host), password(password), nickname("")
{
  std::srand(std::time(NULL));
  quoteManager.loadQuotes();
}

Bot::~Bot()
{
  if (socketFd != -1)
    close(socketFd);
}

void Bot::connectToServer()
{
  socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd == -1)
    throw std::runtime_error("socket failed");

  sockaddr_in server;
  std::memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(host.c_str());

  if (connect(socketFd,
              reinterpret_cast<sockaddr*>(&server),
              sizeof(server)) == -1)
  {
    throw std::runtime_error("connect failed");
  }
  std::cout << "Connected to IRC server at " << host << ":" << port << std::endl;
}

void Bot::sendCommand(const std::string& command)
{
    std::string msg = command + "\r\n";
    if (send(socketFd, msg.c_str(), msg.size(), 0) == -1)
        throw std::runtime_error("send failed");
}

void Bot::login(const std::string& nickname, const std::string& realname)
{
    this->nickname = nickname;
    sendCommand("PASS " + password);
    sendCommand("NICK " + nickname);
    sendCommand("USER " + nickname + " 0 * :" + realname);
}

void Bot::joinChannel(const std::string& channel)
{
    sendCommand("JOIN " + channel);
    std::cout << "Joined channel " << channel << std::endl;
}

void Bot::sendQuote(const std::string& target)
{
    if (quoteManager.isEmpty())
        return;
    Quote q = quoteManager.getRandomQuote();
    std::ostringstream oss;
    oss << "PRIVMSG " << target << " :\"" << q.text << "\" - " << q.author;
    sendCommand(oss.str());
}

void Bot::processLine(const std::string& line)
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;

    std::string l = line;
    if (!l.empty() && l[l.size() - 1] == '\r')
        l.erase(l.size() - 1);

    std::size_t pos = 0;

    if (!l.empty() && l[0] == ':')
    {
        std::size_t sp = l.find(' ');
        if (sp == std::string::npos)
            return;
        prefix = l.substr(1, sp - 1);
        pos = sp + 1;
    }

    std::size_t sp = l.find(' ', pos);
    if (sp == std::string::npos)
    {
        command = l.substr(pos);
        if (command.empty())
            return;
        handleMessage(prefix, command, params);
        return;
    }
    command = l.substr(pos, sp - pos);
    std::string rest = l.substr(sp + 1);

    std::string trail;
    std::size_t colon = rest.find(" :");
    if (colon != std::string::npos)
    {
        std::istringstream iss(rest.substr(0, colon));
        std::string tok;
        while (iss >> tok)
            params.push_back(tok);
        trail = rest.substr(colon + 2);
        params.push_back(trail);
    }
    else
    {
        std::istringstream iss(rest);
        std::string tok;
        while (iss >> tok)
            params.push_back(tok);
    }

    handleMessage(prefix, command, params);
}

void Bot::handleMessage(const std::string& prefix,
                        const std::string& command,
                        const std::vector<std::string>& params)
{
    if (command == "PING")
    {
        if (!params.empty())
            sendCommand("PONG :" + params[0]);
        else
            sendCommand("PONG");
        return;
    }

    if (command == "PRIVMSG" && params.size() >= 2)
    {
        const std::string& target = params[0];
        const std::string& text = params[1];

        std::string upper = text;
        for (std::size_t i = 0; i < upper.size(); ++i)
            upper[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[i])));

        if (text.find("!quote") != std::string::npos)
        {
            std::string replyTarget = target;
            if (target == nickname)
            {
                std::size_t at = prefix.find('!');
                replyTarget = (at != std::string::npos) ? prefix.substr(0, at) : prefix;
            }
            sendQuote(replyTarget);
        }
    }
}

void Bot::run()
{
    char buffer[4096];

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
        {
            std::cout << "Disconnected from server" << std::endl;
            break;
        }

        recvBuffer.append(buffer, bytes);

        std::size_t nl;
        while ((nl = recvBuffer.find('\n')) != std::string::npos)
        {
            std::string line = recvBuffer.substr(0, nl);
            recvBuffer.erase(0, nl + 1);
            processLine(line);
        }
    }
}
