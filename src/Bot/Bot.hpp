#ifndef BOT_HPP
#define BOT_HPP

#include <ctime>
#include <cstdlib>
#include <string>

class Bot{
    public:
        bool isBotCommand(const std::string& msg);
        std::string execute(const std::string& msg);
};

#endif
