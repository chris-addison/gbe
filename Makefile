NAME		=	gbe
TEST_NAME 	= 	test
FLAGS		=	--std=c11
X11			=	-lX11
OPENGL		=	-lGL -lGLU -lm

# run cli by default
all: cli

# cli only
cli:
	gcc $(FLAGS) main.c -o $(NAME)

# experimental x11
x11:
	gcc $(FLAGS) main.c -o $(NAME) -DDISPLAY $(X11) $(OPENGL)

# testing build
	gcc $(FLAGS) test.c -o $(TEST_NAME)
