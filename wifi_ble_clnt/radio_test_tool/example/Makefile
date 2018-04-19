$CC=gcc

main.o: main.c
	gcc -c main.c -g

example: main.o lib_rtt.a
	gcc -o example main.o lib_rtt.a	

all: pre example

pre:
	cp ../obj/lib_rtt.a ./
	cp ../inc/rtt_interface.h ./

clean:
	rm -f *.o example

.PHONY: pre
