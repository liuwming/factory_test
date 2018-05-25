$CC=gcc
ifeq ($(DONUT),y)
CFLAGS += -DDONUT=1
else
CFLAGS += -DDONUT=0
endif

CFLAGS += -DLOG_RESULT

main.o: main.c
	gcc $(CFLAGS) -c main.c -g

example: main.o lib_rtt.a
	gcc $(CFLAGS) -o example main.o lib_rtt.a

all: pre example

pre:
	cp ../radio_test_tool/obj/lib_rtt.a ./
	cp ../radio_test_tool/inc/rtt_interface.h ./

clean:
	rm -f *.o example

.PHONY: pre
