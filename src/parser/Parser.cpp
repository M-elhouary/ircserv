#include "ircserver.hpp"


// this file contains the implementation of the Parser class,
// which is responsible for parsing IRC messages.
// The Parser class provides a static method parse 
// that takes a string input and returns an IRCMessage 
// object containing the parsed components of the message,
// including the prefix, command, and parameters.
// The implementation also includes private helper
// methods to strip CRLF characters, extract the prefix, command,
// and parameters from the input line.


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
  if (pos < line.size() && line[pos] == ':')
  {
    size_t spacePos = line.find(' ', pos);

    if (spacePos != std::string::npos)
    {
      // start from 1 to skip the ':' character because the prefix is optional and starts with ':'
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

  if (pos < line.size())
  {
    size_t spacePos = line.find(' ', pos); // find the next space after the command
    if (spacePos != std::string::npos)
    {

      msg.command = line.substr(pos, spacePos - pos);
      pos = spacePos + 1; // move the position to the character after the space
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
    // If the parameter starts with a colon, it means that the rest of the line is a single parameter
    if (line[pos] == ':')
    {
      std::string temp = line.substr(pos + 1, (line.size() - pos - 1));
      msg.params.push_back(temp);
      break;
    }
    // Otherwise, extract the parameter until the next space
    size_t spacePos = line.find(' ', pos);
    if (spacePos != std::string::npos)
    {

      std::string temp = line.substr(pos, spacePos - pos);
      msg.params.push_back(temp);
      pos = spacePos + 1;
    }
    else
    {
      // If no space is found, the rest of the line is a single parameter
      std::string temp = line.substr(pos);
      msg.params.push_back(temp);
      pos = line.size();
      break;
    }
  }
}


// The parse function is the main entry point for parsing an IRC message.
// returns an IRCMessage object containing the parsed components of the message.
IRCMessage Parser::parse(std::string line)
{
  IRCMessage msg;
  if (!line.empty())
  {
    size_t pos = 0;
    stripCRLF(line); // remove CRLF characters from the end of the line
    extractPrefix(line, msg, pos); // extract the prefix if it exists
    extractCommand(line, msg, pos); // extract the command
    extractParam(line, msg, pos); // extract the parameters
  }
  return msg;
}
