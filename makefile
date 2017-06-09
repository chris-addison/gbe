CC			=	clang
NAME		=	gbe
TEST_NAME 	= 	test
WINDOWS_EXE =	.exe
DIRECTORY	=	src/
FLAGS		=	--std=c11 -Wfatal-errors -Wall -g
X11			=	-lX11
OPENGL		=	-lGL -lGLU -lm
SDL 		=	`sdl2-config --cflags --libs`
DEFINES		=

# Common object to build and include
COMMON		=	gbe.o opcodes.o cartridge.o memory.o debug.o cpu.o screen.o interrupts.o common.o joypad.o display.o
DEBUG 		=	debug.o

# run cli by default
all: sdl

# cli only builds
cli_linux: DEFINES += -DLINUX -DDEBUG
cli_linux: cli.o $(COMMON) $(DEBUG)
	$(CC) $(FLAGS) -o $(NAME) $^

cli_windows: DEFINES += -DWINDOWS -DDEBUG
cli_windows: CC = /usr/bin/x86_64-w64-mingw32-gcc
cli_windows: cli.o $(COMMON) $(DEBUG)
	$(CC) $(FLAGS) -o $(NAME)$(WINDOWS_EXE) $^

# experimental x11
x11: DEFINES += -DLINUX -DOPENGL
x11: x11.o  $(COMMON) gl.o
	$(CC) $(FLAGS) -o $(NAME) $^ $(X11) $(OPENGL)

# experimental sdl
sdl: DEFINES += -DLINUX -DSDL
sdl: sdl.o $(COMMON)
	$(CC) $(FLAGS) -o $(NAME) $^ $(SDL)

# testing builds
test_linux: $(DIRECTORY)testing/test.c $(DIRECTORY)testing/test.h
	$(CC) $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME) -DLINUX

test_windows: $(DIRECTORY)testing/test.c $(DIRECTORY)testing/test.h
	/usr/bin/x86_64-w64-mingw32-gcc $(FLAGS) $(DIRECTORY)testing/test.c -o $(TEST_NAME)$(WINDOWS_EXE) -DWINDOWS


##### Objects #####

cli.o: $(DIRECTORY)frontend/cli/cli.c $(DIRECTORY)frontend/frontend.h
	$(CC) $(FLAGS) $(DIRECTORY)frontend/cli/cli.c -c $(DEFINES)

x11.o: $(DIRECTORY)frontend/x11/x11.c $(DIRECTORY)frontend/frontend.h
	$(CC) $(FLAGS) $(DIRECTORY)frontend/x11/x11.c -c $(DEFINES)

sdl.o: $(DIRECTORY)frontend/sdl/sdl.c $(DIRECTORY)frontend/frontend.h
	$(CC) $(FLAGS) $(DIRECTORY)frontend/sdl/sdl.c -c $(DEFINES)

gbe.o: $(DIRECTORY)gbe.c $(DIRECTORY)gbe.h
	$(CC) $(FLAGS) -c $(DIRECTORY)gbe.c $(DEFINES)

cartridge.o: $(DIRECTORY)cartridge.c $(DIRECTORY)cartridge.h
	$(CC) $(FLAGS) -c $(DIRECTORY)cartridge.c $(DEFINES)

opcodes.o: $(DIRECTORY)opcodes/opcodes.c $(DIRECTORY)opcodes/cb_opcodes.c $(DIRECTORY)opcodes/opcodes.h
	$(CC) $(FLAGS) -c $(DIRECTORY)opcodes/opcodes.c $(DEFINES)

debug.o: $(DIRECTORY)debug/debug.c $(DIRECTORY)debug/debug.h
	$(CC) $(FLAGS) -c $(DIRECTORY)debug/debug.c $(DEFINES)

cpu.o: $(DIRECTORY)cpu.c $(DIRECTORY)cpu.h
	$(CC) $(FLAGS) -c $(DIRECTORY)cpu.c $(DEFINES)

screen.o: $(DIRECTORY)screen.c $(DIRECTORY)screen.h
	$(CC) $(FLAGS) -c $(DIRECTORY)screen.c $(DEFINES)

interrupts.o: $(DIRECTORY)interrupts.c $(DIRECTORY)interrupts.h
	$(CC) $(FLAGS) -c $(DIRECTORY)interrupts.c $(DEFINES)

memory.o: $(DIRECTORY)memory.c $(DIRECTORY)memory.h
	$(CC) $(FLAGS) -c $(DIRECTORY)memory.c $(DEFINES)

common.o: $(DIRECTORY)common.c $(DIRECTORY)common.h
	$(CC) $(FLAGS) -c $(DIRECTORY)common.c $(DEFINES)

display.o: $(DIRECTORY)display.c $(DIRECTORY)display.h
	$(CC) $(FLAGS) -c $(DIRECTORY)display.c $(DEFINES)

joypad.o: $(DIRECTORY)joypad.c $(DIRECTORY)joypad.h
	$(CC) $(FLAGS) -c $(DIRECTORY)joypad.c $(DEFINES)

gl.o: $(DIRECTORY)gfx/gl.c $(DIRECTORY)gfx/gl.h
	$(CC) $(FLAGS) -c $(DIRECTORY)gfx/gl.c $(DEFINES)

clean:
	rm gbe *.o
