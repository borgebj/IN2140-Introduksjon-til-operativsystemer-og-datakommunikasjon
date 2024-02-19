
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = unbuffered buffered

all: $(BIN)

unbuffered: unbuffered.c
	gcc $(CLFAGS) unbuffered.c -o unbuffered

buffered: buffered.c
	gcc $(CLFAGS) buffered.c -o buffered

clean:
	rm -f $(BIN)