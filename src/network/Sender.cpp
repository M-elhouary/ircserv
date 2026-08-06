#include "ircserver.hpp"

int Client::flushSendBuffer()
{
    if (sendBuffer.empty())
        return 0;
    int ret = send(_fd, sendBuffer.c_str(), sendBuffer.size(), 0);
    if (ret > 0) {
        sendBuffer.erase(0, ret);
        return ret;
    }
    return 0;
}
