#include "ircserver.hpp"

const std::string &Client::getRecvBuffer() const { return recv_buffer; }

std::string &Client::getRecvBufferRef() { return recv_buffer; }

void Client::appendToRecvBuffer(const std::string &data) { recv_buffer += data; }

void Client::consumeFromRecvBuffer(size_t n) { recv_buffer.erase(0, n); }
