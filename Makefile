NAME	=	ircserv 
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -std=c++98
RM		=	rm -rf
SRC		=	main.cpp \
			utils/utils.cpp \
			server/Server.cpp \
			server/Client.cpp \
			utils/irc_error.cpp

SRC_PARSED	=	main_test.cpp 
OBJ_PARSED	=	$(SRC_PARSED:.cpp=.o)

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
	printf "${CRED}  _   ${CGRIS} _     ${CYELLOW}_  ${CGREEN}    ${CCYAN}       ${CBLUE} _     ${CPURPLE}     ${CRED}      \n"
	printf "${CRED} | |_ ${CGRIS}| |_  ${CYELLOW}(_)${CGREEN} ___ ${CCYAN} _ __  ${CBLUE}| |_   ${CPURPLE}_ _  ${CRED} __ _ \n"
	printf "${CRED} |  _|${CGRIS}| ' \\ ${CYELLOW}| |${CGREEN}|___|${CCYAN}| '_ \\ ${CBLUE}| ' \\ ${CPURPLE}| ' \\ ${CRED}/ _\` |\n"
	printf "${CRED}  \__|${CGRIS}|_||_|${CYELLOW}|_|${CGREEN}     ${CCYAN}| .__/ ${CBLUE}|_||_|${CPURPLE}|_||_|${CRED}\\__, |\n"
	printf "${CRED}      ${CGRIS}      ${CYELLOW}   ${CGREEN}     ${CCYAN}|_|     ${CBLUE}     ${CPURPLE}      ${CRED}|___/ \n"
	printf "${CYELLOW}"
	@echo Clumsy ${NAME} is on the way!


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

${OBJDIR}/%.o:${SRCDIR}%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	printf "█"

thao: $(OBJ_PARSED)
	@$(CC) $(CFLAGS) $(OBJ_PARSED) -o $(NAME)
	@printf "${B}${CGREEN}ircserv compiled\n"
	# @./$(NAME) 6667 hihitest

launch:
	$(call progress_bar)

clean:
	$(RM) $(OBJDIR)
	@echo "$(B)Cleared$(D)"

fclean:			clean
	$(RM) $(NAME) $(OBJ_PARSED) $(OBJ_PARSED_2)

re:				fclean all

.PHONY:			all clean fclean re launch
