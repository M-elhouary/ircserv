#include "Bot.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./bot <host> <port> <password>" << std::endl;
        return 1;
    }

    std::srand(static_cast<unsigned>(std::time(NULL)));
    try
    {
        Bot bot(argv[1], std::atoi(argv[2]), argv[3]);

        bot.connectToServer();
        bot.login("QuoteBot", "Programming Quote Bot");
        bot.joinChannel("#cpp");
        bot.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Bot error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
