
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = uke4

all: $(BIN)

uke4: uke4.c
	gcc $(CLFAGS) uke4.c -o uke4

clean:
	rm -f $(BIN)

valgrind:
	valgrind ./uke4