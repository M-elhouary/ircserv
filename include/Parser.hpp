#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>   

struct IRCMessage
{
    std::string prefix;              // who sent it  (may be empty)
    std::string command;             // NICK, USER, PASS...
    std::vector<std::string> params; // list of parameters
};

class Parser
{
    public:
        static IRCMessage parse(std::string input);
    private: 
        static void stripCRLF(std::string &line);
        static void extractPrefix(const std::string &line, IRCMessage &msg, size_t &pos);
        static void extractCommand(const std::string &line, IRCMessage &msg, size_t &pos);
        static void extractParam(const std::string &line, IRCMessage &msg, size_t &pos);


};
#endif // PARSER_HPP
