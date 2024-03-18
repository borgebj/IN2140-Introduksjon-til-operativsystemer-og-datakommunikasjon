
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = oppgave1

all: $(BIN)

oppgave1: oppgave1.c
	gcc $(CLFAGS) oppgave1.c -o oppgave1

valgrind:
	valgrind ./oppgave1.c

clean:
	rm -f $(BIN)