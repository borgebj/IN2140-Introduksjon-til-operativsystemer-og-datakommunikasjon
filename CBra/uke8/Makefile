CFLAGS = -std=gnu11 -g -Wall -Wextra
BIN = unbuffered buffered bin_files

all: $(BIN)

unbuffered: unbuffered.c
	gcc $(CLFAGS) unbuffered.c -o unbuffered

buffered: buffered.c
	gcc $(CLFAGS) buffered.c -o buffered

bin_files: bin_files.c
	gcc $(CLFAGS) bin_files.c -o bin_files

clean:
	rm -f $(BIN)