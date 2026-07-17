# Makefile
CC = gcc
CFLAGS = -Wall -Iinclude

all: REPL tripwire

REPL: src/REPL.c src/nittalk.c src/crypto.c
	$(CC) $(CFLAGS) $^ -o REPL

tripwire: src/tripwire.c
	$(CC) $(CFLAGS) $^ -o tripwire

clean:
	rm -f REPL tripwire *.o