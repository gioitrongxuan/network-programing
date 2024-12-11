##
## ETNA PROJECT, 10/02/2020 by habi_a
## c:\Users\habiy\Documents\bomberman\sdl
## File description:
##      Makefile
##

CC		= 	gcc
CFLAGS	+= 	-I/opt/homebrew/include -Wall -Wextra -Werror -Wpedantic
LDFLAGS	=	-lSDL2 -lSDL2main -lSDL2_image

NAME	=	bomberman

SRC		=	src/main.c \
			src/action.c \
			src/app.c \
			src/bag.c \
			src/block.c \
			src/bomb.c \
			src/check_move.c \
			src/game.c \
			src/map.c \
			src/move.c \
			src/player.c

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
