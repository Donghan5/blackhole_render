NAME	= blackhole

CC		= gcc
CFLAGS	= -Wall -Wextra -Werror -O2 -std=c99 $(shell sdl2-config --cflags)
LDFLAGS	= $(shell sdl2-config --libs) -lm -lavif

SRCS	= src/main.c \
		  src/math/vec3.c \
		  src/ray/ray.c \
		  src/camera/camera.c \
		  src/background/background.c \
		  src/render/render.c \
		  src/rk4/rk4.c \
		  src/geodesic/geodesic.c \
		  src/accretion/accretion.c

TEST_RK4		= test_rk4
TEST_RK4_SRCS	= src/rk4/rk4.c src/rk4/test_rk4.c

TEST_GEODESIC		= test_geodesic
TEST_GEODESIC_SRCS	= src/rk4/rk4.c src/geodesic/geodesic.c \
					  src/geodesic/test_geodesic.c

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

TEST_ACCRETION		= test_accretion
TEST_ACCRETION_SRCS	= src/accretion/accretion.c \
					  src/accretion/test_accretion.c

fclean: clean
	rm -f $(NAME) $(TEST_RK4) $(TEST_GEODESIC) $(TEST_ACCRETION)

$(TEST_RK4): $(TEST_RK4_SRCS)
	$(CC) -Wall -Wextra -Werror -std=c99 -O2 $(TEST_RK4_SRCS) -lm -o $(TEST_RK4)

$(TEST_GEODESIC): $(TEST_GEODESIC_SRCS)
	$(CC) -Wall -Wextra -Werror -std=c99 -O2 $(TEST_GEODESIC_SRCS) -lm -o $(TEST_GEODESIC)

$(TEST_ACCRETION): $(TEST_ACCRETION_SRCS)
	$(CC) -Wall -Wextra -Werror -std=c99 -O2 $(TEST_ACCRETION_SRCS) -lm -o $(TEST_ACCRETION)

test: $(TEST_RK4) $(TEST_GEODESIC) $(TEST_ACCRETION)
	@echo "--- RK4 Tests ---"
	./$(TEST_RK4)
	@echo ""
	@echo "--- Geodesic Tests ---"
	./$(TEST_GEODESIC)
	@echo ""
	@echo "--- Accretion Disk Tests ---"
	./$(TEST_ACCRETION)

re: fclean all

.PHONY: all clean fclean re test
