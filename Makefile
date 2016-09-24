NAME=test
FLAGS=--std=c11
GTK=`pkg-config --cflags --libs gtk+-3.0`

# run cli by default
all: cli

# cli only
cli:
	gcc $(FLAGS) main.c -o $(NAME)

# gtk
gtk:
	gcc $(FLAGS) main.c -o $(NAME) -DDISPLAY $(GTK)
