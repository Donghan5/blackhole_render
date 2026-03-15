NAME	= blackhole

CC		= gcc
CFLAGS	= -Wall -Wextra -Werror -O2 -std=c99 $(shell sdl2-config --cflags)
LDFLAGS	= $(shell sdl2-config --libs) -lm

SRCS	= src/main.c \
		  src/math/vec3.c \
		  src/ray/ray.c \
		  src/camera/camera.c \
		  src/background/background.c \
		  src/render/render.c

OBJDIR	= obj
OBJS	= $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
