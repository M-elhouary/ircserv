#include "ircserver.hpp"

// remove \r\\n from the end of the line
void Parser::stripCRLF(std::string &line)
{
  if (!line.empty() && line[line.size() - 1] == '\n')
    line.erase((line.size()) - 1);

  if (!line.empty() && line[line.size() - 1] == '\r')
    line.erase((line.size()) - 1);
}

void Parser::extractPrefix(const std::string &line, IRCMessage &msg,
                           size_t &pos)
{
  if (line[pos] == ':')
  {
    size_t spacePos = line.find(' ', pos);

    if (spacePos != std::string::npos)
    {
      msg.prefix = line.substr(1, spacePos - 1);
      pos = spacePos + 1;
    }
    else
    {
      msg.prefix = line.substr(1);
      pos = line.size();
    }
  }
}

void Parser::extractCommand(const std::string &line, IRCMessage &msg,
                            size_t &pos)
{

  if (pos != 0 && !line.empty())
  {
    size_t spacePos = line.find(' ');
    if (spacePos != std::string::npos)
    {

      msg.command = line.substr(pos, spacePos - pos);
      pos = spacePos + 1;
    }
    else
    {
      msg.command = line.substr(pos);
      pos = line.size();
    }
  }
}

void Parser::extractParam(const std::string &line, IRCMessage &msg,
                          size_t &pos)
{
  while (pos < line.size())
  {
    if (line[pos] == ':')
    {
      std::string temp = line.substr(pos + 1, (line.size() - pos - 1));
      msg.params.push_back(temp);
      break;
    }

    size_t spacePos = line.find(' ', pos);
    if (spacePos != std::string::npos)
    {

      std::string temp = line.substr(pos, spacePos - pos);
      msg.params.push_back(temp);
      pos = spacePos + 1;
    }
    else
    {

      std::string temp = line.substr(pos);
      msg.params.push_back(temp);
      pos = line.size();
      break;
    }
  }
}

IRCMessage Parser::parse(std::string line)
{
  IRCMessage msg;
  if (!line.empty())
  {
    size_t pos = 0;
    stripCRLF(line);
    extractPrefix(line, msg, pos);
    extractCommand(line, msg, pos);
    extractParam(line, msg, pos);
  }
  return msg;
}
