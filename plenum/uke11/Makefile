all: interfaces arpjob cidr

cidr: cidr.o
	gcc -g -o $@ $^

arpjob: arpjob.o
	gcc -g -o $@ $^

interfaces: interfaces.o af_map.o
	gcc -g -o $@ $^

%.o: %.c
	gcc -c $^

interface.o: af_map.h
af_map.o: af_map.h

clean:
	rm -f interfaces
	rm -r arpjob
	rm -r cidr
	rm -f *.o
	rm -f *.gch
