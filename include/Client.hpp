#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {


    private:
        int         _fd;
        std::string recv_buffer;
        std::string _nickname;
        std::string _username;
        bool        _authenticated;
        bool        _nicknameReceived;
        bool        _registred;
        bool        _welcomeSent;

    public:
        Client(int fd);
        ~Client();
        void setNickName(const std::string nickname);
        void setUserName(const std::string username);
        void setAutenticated(bool _authenticate);
        void setNickNameReceived(bool _nicknameReceived);
        void setRegistred(bool _registred);
        bool getWelcomeSent() const;
        void setWelcomeSent(bool value);
        bool getAutenticated();
        std::string getNickName();
        std::string getUserName();

        int getFd() const;

        const std::string &getRecvBuffer() const;
        std::string &getRecvBufferRef();
        void appendToRecvBuffer(const std::string &data);
        void consumeFromRecvBuffer(size_t n);

        bool isRegistred() const;
        void sendMessage(std::string message);
};

#endif
