CC = cc

FLAGS = -Wall -Werror -Wextra -pedantic
DEBUG = -g

OBJECTS = bloop.c

all: main

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o bloop $(FLAGS)

debug: $(OBJECTS)
	$(CC) $(OBJECTS) $(DEBUG) -D DEBUG -o bloop
