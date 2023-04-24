NAME	=	ircserv 
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -g3 -std=c++98 -I include/
TESTFLAGS =   -I lib/googletest/include -L lib/googletest/lib -lgtest -lgtest_main -lgmock
RM		=	rm -rf

SRC		=	server/Server.cpp \
			server/Client.cpp \
			server/Parser.cpp \
			server/Command.cpp \
			server/CommandParser.cpp \
			server/TargetParser.cpp \
			server/ChannelParser.cpp \
			server/Channel.cpp \
			server/Ban.cpp \
			utils/utils.cpp \
			utils/irc_error.cpp \
			main.cpp

TEST	=	tests/parser_test.cpp

OBJDIR	=	objs
OBJ		=	$(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

.SILENT:
SHELL	:= bash
CRED 	= \033[0;31m
CGRIS	= \033[0;214m
CWHITE	= \033[0;37m
CYELLOW	= \033[1;33m
CGREEN 	= \033[0;32m
CCYAN 	= \033[1;36m
CBLUE 	= \033[0;34m
CPURPLE	= \033[0;35m
B 	=	$(shell tput bold)
D =		$(shell tput sgr0)


all: TITLE launch $(NAME)
	@printf "\n$(B)$(CCYAN)$(NAME) compiled$(D)\n\n"

TITLE:

	printf "${CRED}.----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n"
	printf "${CGRIS}| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n"
	printf "${CWHITE}| |  _________   | || |  _________   | || |              | || |     _____    | || |  _______     | || |     ______   | |\n"
	printf "${CYELLOW}| | |_   ___  |  | || | |  _   _  |  | || |              | || |    |_   _|   | || | |_   __ \    | || |   .' ___  |  | |\n"
	printf "${CGREEN}| |   | |_  \_|  | || | |_/ | | \_|  | || |              | || |      | |     | || |   | |__) |   | || |  / .'   \_|  | |\n"
	printf "${CCYAN}| |   |  _|      | || |     | |      | || |              | || |      | |     | || |   |  __ /    | || |  | |         | |\n"
	printf "${CBLUE}| |  _| |_       | || |    _| |_     | || |              | || |     _| |_    | || |  _| |  \ \_  | || |  \ \`.___.'\  | |\n"
	printf "${CPURPLE}| | |_____|      | || |   |_____|    | || |   _______    | || |    |_____|   | || | |____| |___| | || |   \`._____.'  | |\n"
	printf "${CRED}| |              | || |              | || |  |_______|   | || |              | || |              | || |              | |\n"
	printf "${CYELLOW}| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n"
	printf "${CCYAN} '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n${CGREEN}"
	printf "${CYELLOW}Clumsy ${CPURPLE}${NAME}${CYELLOW}is on the way!\n"
	printf "${CGREEN}Made with 💌${CRED}l${CYELLOW}o${CCYAN}v${CGRIS}e${CGREEN}💌 by ___ }@jfrancai @qbornet and @thi-phng ___\n"



 # ******************************************************************************* #
define  progress_bar
        @i=0; \
        while [[ $$i -le $(words $(SRCS)) ]]; do \
                printf " "; \
                ((i = i + 1)); \
        done; \
		printf "$(B)$(CWHITE)]\r[$(CGREEN)";
endef
 # ******************************************************************************* #

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@printf "${B}${CWHITE}]\n"

bonus:
	@pip install -r ./requirements.txt
	@python3 bot.py

${OBJDIR}/%.o:${SRCDIR}%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	printf "█"

test: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) $(TESTFLAGS) -o $(NAME)
	@printf "${B}${CWHITE}]\n"

launch:
	$(call progress_bar)

clean:
	$(RM) $(OBJDIR)
	@echo "$(B)Cleared$(D)"

fclean:			clean
	$(RM) $(NAME) $(OBJ_PARSED) $(OBJ_PARSED_2)

re:				fclean all

.PHONY:			all clean fclean re launch bonus
