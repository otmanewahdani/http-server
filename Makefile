CC = c++
CPPFLAGS = -std=c++98 -c -Wall -Wextra -Werror

ifeq ($(shell uname), Darwin)
SHELL = /bin/zsh
else
SHELL = /bin/bash
endif

OBJ_DIR = objs/

SRCS_DIR = srcs/

CONFIG_SRC := Config.cpp ConfigParser.cpp ConfigLexer.cpp

GENERAL_SRC := utils.cpp MimeTypes.cpp main.cpp

SRCS := $(CONFIG_SRC) $(GENERAL_SRC) 

VPATH = $(patsubst %.cpp,%/,$(SRCS) ) 

VPATH := $(addprefix $(SRCS_DIR),$(VPATH) )

INCS = $(addprefix -I,$(VPATH))

OBJ = $(addprefix $(OBJ_DIR), $(patsubst %.cpp,%.o,$(SRCS)) )

NAME = webserv

all: $(NAME)

$(OBJ_DIR)%.o : %.cpp %.hpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(INCS) $< -o $@

$(OBJ_DIR)main.o: $(SRCS_DIR)main.cpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(INCS) $< -o $@

$(NAME): $(OBJ)
	@$(CC) $^ -o $@
	@echo -e "\e[1;35m\u2705 Web server was created successfully\e[0m"

clean:
	@rm -fr $(OBJ) $(OBJ_DIR)
	@echo -e "\e[1;31m\u26A0 all object files were removed permanently\e[0m"

fclean: clean
	@rm -f $(NAME)
	@echo -e "\e[1;31m\u26A0 full cleaning complete\e[0m"

re: fclean all
	@echo -e "\e[1;32m\u2705 all targets were re-created!\e[0m"

.PHONY: all clean fclean re
