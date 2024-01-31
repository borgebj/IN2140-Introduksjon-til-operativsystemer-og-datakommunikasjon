all:scheduler sl_list

scheduler:scheduler.c
	gcc -std=gnu11 scheduler.c -o scheduler

scheduler:sl_list.c
	gcc -std=gnu11 sl_list.c -o sl_list

clean:
	del -f scheduler.exe sl_list.exe