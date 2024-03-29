SRC = main.cpp Config.cpp Utils.cpp

SRC_DIR = src/
OBJ_DIR = obj/

OSRC = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

CFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g

NAME = webserv

RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
RESET = \033[0m

all: $(NAME)
	@clear && printf '\e[3J'
# $(NAME): ascci_art $(OSRC)
$(NAME): $(OSRC)
	@c++ $(CFLAGS) $(OSRC) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	@c++ $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean:
	@rm -rf $(OBJ_DIR) $(NAME)

re:fclean all

ascci_art:
	@clear
	@printf "$(GREEN)                                                                                                        \n\
                                                                                                        \n\
                                                                                                        \n\
                                                          bbbbbbbb                                                                                             \n\
WWWWWWWW                           WWWWWWWW               b::::::b               SSSSSSSSSSSSSSS                                                               \n\
W::::::W                           W::::::W               b::::::b             SS:::::::::::::::S                                                              \n\
W::::::W                           W::::::W               b::::::b            S:::::SSSSSS::::::S                                                              \n\
W::::::W                           W::::::W                b:::::b            S:::::S     SSSSSSS                                                              \n\
 W:::::W           WWWWW           W:::::W eeeeeeeeeeee    b:::::bbbbbbbbb    S:::::S                eeeeeeeeeeee    rrrrr   rrrrrrrrrvvvvvvv           vvvvvvv\n\
  W:::::W         W:::::W         W:::::Wee::::::::::::ee  b::::::::::::::bb  S:::::S              ee::::::::::::ee  r::::rrr:::::::::rv:::::v         v:::::v \n\
   W:::::W       W:::::::W       W:::::We::::::eeeee:::::eeb::::::::::::::::b  S::::SSSS          e::::::eeeee:::::eer:::::::::::::::::rv:::::v       v:::::v  \n\
    W:::::W     W:::::::::W     W:::::We::::::e     e:::::eb:::::bbbbb:::::::b  SS::::::SSSSS    e::::::e     e:::::err::::::rrrrr::::::rv:::::v     v:::::v   \n\
     W:::::W   W:::::W:::::W   W:::::W e:::::::eeeee::::::eb:::::b    b::::::b    SSS::::::::SS  e:::::::eeeee::::::e r:::::r     r:::::r v:::::v   v:::::v    \n\
      W:::::W W:::::W W:::::W W:::::W  e:::::::::::::::::e b:::::b     b:::::b       SSSSSS::::S e:::::::::::::::::e  r:::::r     rrrrrrr  v:::::v v:::::v     \n\
       W:::::W:::::W   W:::::W:::::W   e::::::eeeeeeeeeee  b:::::b     b:::::b            S:::::Se::::::eeeeeeeeeee   r:::::r               v:::::v:::::v      \n\
        W:::::::::W     W:::::::::W    e:::::::e           b:::::b     b:::::b            S:::::Se:::::::e            r:::::r                v:::::::::v       \n\
         W:::::::W       W:::::::W     e::::::::e          b:::::bbbbbb::::::bSSSSSSS     S:::::Se::::::::e           r:::::r                 v:::::::v        \n\
          W:::::W         W:::::W       e::::::::eeeeeeee  b::::::::::::::::b S::::::SSSSSS:::::S e::::::::eeeeeeee   r:::::r                  v:::::v         \n\
           W:::W           W:::W         ee:::::::::::::e  b:::::::::::::::b  S:::::::::::::::SS   ee:::::::::::::e   r:::::r                   v:::v          \n\
            WWW             WWW            eeeeeeeeeeeeee  bbbbbbbbbbbbbbbb    SSSSSSSSSSSSSSS       eeeeeeeeeeeeee   rrrrrrr                    vvv           \n\
                                                                                                        \n\
                                                                      $(RED)    by: abouabra\n\
                                                                                                        \n$(RESET)"
                                                                                                  
.PHONY: all clean fclean re
