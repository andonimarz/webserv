NAME			=	webserv
SRC_DIR			=	src
OBJ_DIR 		=	obj
INC_DIR			=	include

RM				=	rm -rf
CC				=	c++
CFLAGS			=	-I $(INC_DIR) -Wall -Werror -Wextra  -g3 -fsanitize=address #-std=c++98

SRCS			=	$(SRC_DIR)/webserver.cpp	\
					$(SRC_DIR)/Route.cpp		\
					$(SRC_DIR)/Config.cpp		\
					$(SRC_DIR)/Server.cpp		\
					$(SRC_DIR)/utils.cpp		\
					$(SRC_DIR)/Request.cpp		\
					$(SRC_DIR)/Response.cpp		\
					$(SRC_DIR)/utils_config.cpp	\
					$(SRC_DIR)/tests.cpp		\

OBJS			=	$(SRCS:.cpp=.o)
PREFIXED		=	$(addprefix $(OBJ_DIR)/, $(OBJS))

RED				=	"\\x1b[31m"
GREEN			=	"\\x1b[32m"
YELLOW			=	"\\x1b[33m"
RESET			=	"\\x1b[37m"

# =================================== RULES ===================================

$(NAME):	$(PREFIXED)
	@printf \
	"$(YELLOW)-------------- Project's files compiled --------------\n$(RESET)\n%-33.33s\r"
	@$(CC) $(CFLAGS) $(PREFIXED) -o $(NAME)
	@echo "\n${GREEN}-------------- Mandatory part compiled! --------------${RESET}\n"

$(OBJ_DIR)/%.o:	%.cpp
	@mkdir -p $(OBJ_DIR)/$(SRC_DIR)
	@printf "$(YELLOW)Compiling project's file -> $(RESET)%-33.33s\r" $<
	@$(CC) $(CFLAGS) -c $< -o $@

# ================================= FUNCTIONS =================================

all:	$(NAME)

clean:
	@$(RM) $(OBJ) $(OBJ_DIR)
	@echo "\n${GREEN}---------------------- Cleaned! ----------------------${RESET}\n"

fclean:	clean
	@$(RM) $(NAME) $(NAME).dSYM

re:		fclean all

.PHONY: all clean fclean re bonus print_init