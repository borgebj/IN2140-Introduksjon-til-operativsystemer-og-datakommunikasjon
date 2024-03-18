
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = oppgave1 oppgave2

all: $(BIN)

oppgave1: oppgave1.c
	gcc $(CLFAGS) oppgave1.c -o oppgave1

oppgave2: oppgave2.c
	gcc $(CFLAGS) oppgave2.c -o oppgave2

valgrind:
	valgrind ./oppgave1.c

clean:
	rm -f $(BIN)