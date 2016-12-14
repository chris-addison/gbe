CC			=	clang
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
cli_linux: gbe.o opcodes.o memory.o cartridge.o debug.o cpu.o screen.o interrupts.o common.o
	$(CC) $(FLAGS) $(DIRECTORY)frontend/cli/cli.c -c $(DEFINES)
	$(CC) $(FLAGS) -o $(NAME) cli.o $^
	rm *.o

cli_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)main.c -o $(NAME)$(WINDOWS_EXE) -DWINDOWS

# experimental x11
x11: DEFINES += -DDISPLAY -DLINUX -DX11 -DOPENGL
x11: gbe.o opcodes.o cartridge.o memory.o debug.o cpu.o screen.o interrupts.o common.o display.o
	$(CC) $(FLAGS) $(DIRECTORY)frontend/x11/x11.c -c $(DEFINES)
	$(CC) $(FLAGS) -o $(NAME) x11.o $^ $(X11) $(OPENGL)
	rm *.o

# experimental sdl
sdl:
	$(CC) $(FLAGS) $(DIRECTORY)main.c -o $(NAME) -DDISPLAY -DLINUX -DSDL $(SDL)

# testing builds
test_linux:
	$(CC) $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME) -DLINUX

test_windows:
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME)$(WINDOWS_EXE) -DWINDOWS

gbe.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)gbe.c $(DEFINES)

cartridge.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)cartridge.c $(DEFINES)

opcodes.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)opcodes/opcodes.c $(DEFINES)

debug.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)debug/debug.c $(DEFINES)

cpu.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)cpu.c $(DEFINES)

screen.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)screen.c $(DEFINES)

interrupts.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)interrupts.c $(DEFINES)

memory.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)memory.c $(DEFINES)

common.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)common.c $(DEFINES)

display.o:
	$(CC) $(FLAGS) -c $(DIRECTORY)display.c $(DEFINES)

clean:
	rm gbe *.o
