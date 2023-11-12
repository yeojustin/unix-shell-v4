CC = gcc
CFLAGS = -Wall -Wextra -g

all: shell

shell: main.o command.o shell.o
	$(CC) $(CFLAGS) -o shell main.o command.o shell.o -lm

main.o: main.c shell.h command.h
	$(CC) $(CFLAGS) -c main.c

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c

shell.o: shell.c shell.h command.h
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f shell *.o