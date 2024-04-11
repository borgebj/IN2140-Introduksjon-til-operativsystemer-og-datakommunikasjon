
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = server client

all: $(BIN)

server: server.c
	gcc $(CLFAGS) server.c -o server

client: client.c
	gcc $(CLFAGS) client.c -o client

valgrind:
	valgrind ./client ./server

clean:
	rm -f $(BIN)