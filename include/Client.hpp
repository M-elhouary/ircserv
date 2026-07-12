#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <string>



class Client {


    private:
        int         _fd;
        std::string _nickname;
        std::string _username;
        bool        _authenticated;
        bool        _nicknameReceived;
        bool        _registred;

    public:
        Client(int fd);
        ~Client();
        void setNickName(const std::string nickname);
        void setUserName(const std::string username);
        void setAutenticated(bool _authenticate);
        void setNickNameReceived(bool _nicknameReceived);
        void setRegistred(bool _registred);
        bool getAutenticated();
        std::string getNickName();
        std::string getUserName();
        int getFd() const;
        bool isRegistred() const;
        void sendMessage(std::string message);
};


#endif
