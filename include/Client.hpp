#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {

    private:
        int         _fd;
        std::string recv_buffer;
        std::string sendBuffer;
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        bool        _authenticated;
        bool        _nicknameReceived;
        bool        _registred;
        bool        _welcomeSent;
        bool       _passwordReceived;

    public:
        Client(int fd);
        ~Client();
        void setNickName(const std::string nickname);
        void setUserName(const std::string username);
        void setAutenticated(bool _authenticate);
        void setNickNameReceived(bool _nicknameReceived);
        void setRegistred(bool _registred);
        void setHostname(struct sockaddr_in client_addr);
        void setWelcomeSent(bool value);
        void setPasswordReceived(bool value);
        bool getWelcomeSent() const;
        bool getNickNameReceived() const;
        bool getAutenticated();
        bool getPasswordReceived() const;
        std::string getNickName();
        std::string getUserName();

        int getFd() const;
        std::string getHostname() const;

        const std::string &getRecvBuffer() const;
        std::string &getRecvBufferRef();
        void appendToRecvBuffer(const std::string &data);
        void consumeFromRecvBuffer(size_t n);

        bool isRegistred() const;
        void sendMessage(const std::string &message);
        bool hasPendingSend() const;
        int flushSendBuffer();
};

#endif
