CC = cc

FLAGS = -Wall -Werror -pedantic
DEBUG = -g

OBJECTS = bloop.c

all: main

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o bloop $(FLAGS)

debug: $(OBJECTS)
	$(CC) $(OBJECTS) $(DEBUG) -o bloop
