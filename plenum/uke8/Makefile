all: first_fit test_list

first_fit: first_fit.o sl_list.o
	gcc -g -o first_fit first_fit.o sl_list.o

test_list: test_list.o sl_list.o
	gcc -g -o test_list test_list.o sl_list.o

first_fit.o: first_fit.c sl_list.h
	gcc -g -c first_fit.c

test_list.o: test_list.c sl_list.h
	gcc -g -c test_list.c

sl_list.o: sl_list.c sl_list.h
	gcc -g -c sl_list.c

clean:
	rm -f *.o

realclean: clean
	rm -f test_list
	rm -f first_fit

