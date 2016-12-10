NAME		=	gbe
TEST_NAME 	= 	test
WINDOWS_EXE =	.exe
DIRECTORY	=	src/
FLAGS		=	--std=c11 -Wfatal-errors -Wall
X11			=	-lX11
OPENGL		=	-lGL -lGLU -lm
SDL 		=	`sdl2-config --cflags --libs`
DEFINES		=

# run cli by default
all: cli_linux

# cli only builds
cli_linux: DEFINES += -DLINUX -DDEBUG
cli_linux: opcodes.o debug.o cpu.o screen.o interrupts.o common.o
	gcc $(FLAGS) $(DIRECTORY)main.c -c $(DEFINES)
	gcc $(FLAGS) -o $(NAME) main.o $^
	rm *.o

cli_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)main.c -o $(NAME)$(WINDOWS_EXE) -DWINDOWS

# experimental x11
x11: DEFINES += -DDISPLAY -DLINUX -DX11 -DOPENGL
x11: opcodes.o debug.o cpu.o screen.o interrupts.o common.o display.o
	gcc $(FLAGS) $(DIRECTORY)main.c -c $(DEFINES)
	gcc $(FLAGS) -o $(NAME) main.o $^ $(X11) $(OPENGL)
	rm *.o

# experimental sdl
sdl:
	gcc $(FLAGS) $(DIRECTORY)main.c -o $(NAME) -DDISPLAY -DLINUX -DSDL $(SDL)

# testing builds
test_linux:
	gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME) -DLINUX

test_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME)$(WINDOWS_EXE) -DWINDOWS

opcodes.o:
	gcc $(FLAGS) -c $(DIRECTORY)opcodes/opcodes.c $(DEFINES)

debug.o:
	gcc $(FLAGS) -c $(DIRECTORY)debug/debug.c $(DEFINES)

cpu.o:
	gcc $(FLAGS) -c $(DIRECTORY)cpu.c $(DEFINES)

screen.o:
	gcc $(FLAGS) -c $(DIRECTORY)screen.c $(DEFINES)

interrupts.o:
	gcc $(FLAGS) -c $(DIRECTORY)interrupts.c $(DEFINES)

common.o:
	gcc $(FLAGS) -c $(DIRECTORY)common.c $(DEFINES)

display.o:
	gcc $(FLAGS) -c $(DIRECTORY)display.c $(DEFINES)

clean:
	rm gbe *.o
