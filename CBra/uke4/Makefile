
CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = hvaPrintes hello greet

all: $(BIN)

hvaPrintes: hvaPrintes.c
	gcc -std=gnu11 -g -Wall -Wextra hvaPrintes.c -o hvaPrintes

hello: hello.c
	gcc $(CLFAGS) hello.c -o hello

greet: demo.c func.c
	gcc $(CFLAGS) demo.c func.c -o greet

clean:
	rm -f $(BIN)