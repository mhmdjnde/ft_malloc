# *************************************************#
#                                                  #
#                                                  #
#   Makefile                                       #
#                                                  #
#   By: JndeIsBack                                 #
#                                                  #
#   Created: 2026/09/10 00:00:00 by JndeIsBack     #
#   Updated: 2029/11/99 23:33:56 by JndeIsBack     #
#                                                  #
# *************************************************#

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME    = libft_malloc_$(HOSTTYPE).so
LINK    = libft_malloc.so

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g -fPIC
LDFLAGS = -shared

SRC     = srcs/malloc.c srcs/free.c srcs/realloc.c srcs/show_alloc_mem.c \
          srcs/align_and_size.c srcs/zone.c
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(NAME) $(OBJ)
	ln -sf $(NAME) $(LINK)

%.o: %.c srcs/ft_malloc.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(LINK)

re: fclean all

.PHONY: all clean fclean re
