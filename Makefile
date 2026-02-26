NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS = srcs/main.cpp \
       srcs/Server.cpp \
       srcs/Client.cpp \
       srcs/Channel.cpp \
       srcs/Commands.cpp \
	   srcs/PreRegister.cpp \
	   srcs/signals.cpp \
	   srcs/CommandJoin.cpp \
	   srcs/CommandMode.cpp \
	   srcs/CommandsOthers.cpp


OBJDIR = objs
OBJS = $(patsubst srcs/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: srcs/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
