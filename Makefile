NAME		=	gbe
TEST_NAME 	= 	test
WINDOWS_EXE =	.exe
DIRECTORY	=	src/
FLAGS		=	--std=c11
X11			=	-lX11
OPENGL		=	-lGL -lGLU -lm
SDL 		=	`sdl2-config --cflags --libs`

# run cli by default
all: cli_linux

# cli only builds
cli_linux: opcodes.o
	gcc $(FLAGS) $(DIRECTORY)main.c -c -DLINUX
	gcc $(FLAGS) -o $(NAME) main.o opcodes.o

cli_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)main.c -o $(NAME)$(WINDOWS_EXE) -DWINDOWS

# experimental x11
x11: opcodes.o
	gcc $(FLAGS) $(DIRECTORY)main.c -c -DDISPLAY -DLINUX -DX11 -DOPENGL
	gcc $(FLAGS) -o $(NAME) main.o opcodes.o $(X11) $(OPENGL)

# experimental sdl
sdl:
	gcc $(FLAGS) $(DIRECTORY)main.c -o $(NAME) -DDISPLAY -DLINUX -DSDL $(SDL)

# testing builds
test_linux:
	gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME) -DLINUX

test_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME)$(WINDOWS_EXE) -DWINDOWS

opcodes.o:
	gcc $(FLAGS) -c $(DIRECTORY)/opcodes/opcodes.c
