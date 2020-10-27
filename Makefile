CC = gcc
CFLAGS = -g -Wall

default: pseudu-shell

pseudu-shell: main.o command.o
	$(CC) $(CFLAGS) -o pseudu-shell main.o command.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c

clean:
	$(RM) pseudu-shell *.o *~
