#include "Bot.hpp"

bool    Bot::isBotCommand(const std::string& msg){
        return((msg.find("!bot") == 0) || (msg.find("!Bot") == 0));
}

std::string Bot::execute(const std::string& msg){
        if (msg.size() <= 4)
                return ("‼️ Unknown bot command.Try !bot help‼️"); 
        std::string command = msg.substr(5);
        if (command == "help")
                return ("🤖Command: help, hello, ping, time, joke🤖");
        else if(command == "hello")
                return ("hy, I am your IRC bot!😊");
        else if (command == "ping")
                return ("pong🏓");
        else if (command == "time"){
                time_t now = time(0);
                return (ctime(&now));
        }
        else if (command == "joke"){
                const std::string jokes[] = {
                "Why do programmers hate nature? Too many bugs.😂",
                "Debugging: being the detective in a crime movie where you are also the murderer.😂",
                "There are 10 types of people: those who understand binary and those who don't.😂"
                };
                return (jokes[rand() % 3]);
        }
        return ("‼️ Unknown bot command.Try !bot help‼️");
}
