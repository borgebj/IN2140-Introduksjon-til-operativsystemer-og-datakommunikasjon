
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = msn

all: $(BIN)

server: server.c
	gcc $(CLFAGS) msn.c -o msn

valgrind:
	valgrind ./msn

clean:
	rm -f $(BIN)