NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

SRCS = main.cpp \
       src/server/Server.cpp \
       src/Client/Client.cpp \
       src/Channel/Channel.cpp \
       src/Commands/Pass.cpp \
       src/Commands/Nick.cpp \
       src/Commands/User.cpp \
       src/Commands/Join.cpp \
       src/Commands/Part.cpp \
       src/Commands/Privmsg.cpp \
       src/Commands/topic.cpp \
       src/Commands/invite.cpp \
       src/Commands/kick.cpp \
       src/Commands/mode.cpp \
       src/Commands/manual.cpp \
       src/Bot/Bot.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
