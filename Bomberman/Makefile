##
## ETNA PROJECT, 10/02/2020 by habi_a
## c:\Users\habiy\Documents\bomberman\sdl
## File description:
##      Makefile
##

CC		= 	gcc
CFLAGS	+= 	-I/opt/homebrew/include -L/opt/homebrew/lib -Wall -Wextra  -Wpedantic
LDFLAGS	=	-lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf

NAME	=	bomberman

SRC		=	src/main.c \
			src/action.c \
			src/app.c \
			src/bag.c \
			src/block.c \
			src/bomb.c \
			src/button.c \
			src/check_move.c \
			src/client.c \
			src/client_connect.c \
			src/client_state.c \
			src/decode.c \
			src/encode.c \
			src/explosion.c \
			src/explosions.c \
			src/game.c \
			src/map.c \
			src/menu_state.c \
			src/move.c \
			src/player.c \
			src/server.c \
			src/server_connect.c \
			src/server_state.c \
			src/server_users.c

OBJ		= 	$(SRC:.c=.o)

RM		= 	rm -f


all:		$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: 	clean
	$(RM) $(NAME)

re:			fclean all

.PHONY	= 	all $(NAME) clean fclean re
