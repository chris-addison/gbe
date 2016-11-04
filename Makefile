NAME		=	gbe
TEST_NAME 	= 	test
WINDOWS_EXE =	.exe
FLAGS		=	--std=c11
X11			=	-lX11
OPENGL		=	-lGL -lGLU -lm

# run cli by default
all: cli_linux

# cli only builds
cli_linux:
	gcc $(FLAGS) main.c -o $(NAME) -DLINUX

cli_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) main.c -o $(NAME)$(WINDOWS_EXE) -DWINDOWS

# experimental x11
x11:
	gcc $(FLAGS) main.c -o $(NAME) -DDISPLAY $(X11) $(OPENGL)

# testing builds
test_linux:
	gcc $(FLAGS) test.c -o $(TEST_NAME) -DLINUX

test_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) test.c -o $(TEST_NAME)$(WINDOWS_EXE) -DWINDOWS
