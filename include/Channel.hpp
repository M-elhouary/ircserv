#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Channel {

        private:
            std::string _channelName;
            std::vector<Client *> _clients;
            std::vector<std::string> _operators;
        public:
            Channel(std::string name);
            ~Channel();
            std::string getName() const;
            void addClient(Client *client);
            void removeClient(Client *client);
            bool isClientInChannel(Client *client) const;
            void addOperator(const std::string &nickname);
            void removeOperator(const std::string &nickname);
            bool isOperator(const std::string &nickname) const;

};

#endif // CHANNEL_HPP