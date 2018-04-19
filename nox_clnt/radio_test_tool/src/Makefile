CC=gcc
AR=ar

CFLAGS=-g -Wall -fPIC

ifeq ($(DONUT),y)
CFLAGS += -DDONUT=1
else
CFLAGS += -DDONUT=0
endif

ifeq ($(CHERRY1S),y)
CFLAGS += -DCHERRY1S=1
else
CFLAGS += -DCHERRY1S=0
endif


INCLUDE=-I. -I../inc

TARGETDIR=../obj

OBJS=main.o util.o iomux.o cmd.o timer.o test.o rtt_interface.o\
     linux.o linux_tap.o air.o osdep.o common.o radiotap.o \
     lib_aes.o lib_md5.o miio_proto.o sbox.o

LIB_OBJS=rtt_interface.o

SS_OBJ=sssvr.o

rtt: $(OBJS)
	cd $(TARGETDIR); $(CC) $(CFLAGS) -o rtt $(INCLUDE) $^ -lrt -lpthread

lib: $(LIB_OBJS)
	cd $(TARGETDIR); ar -r lib_rtt.a $(LIB_OBJS)

sssvr: $(SS_OBJ)
	cd $(TARGETDIR); $(CC) $(CFLAGS) -o sssvr $(INCLUDE) $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(TARGETDIR)/$@

all: rtt lib sssvr

clean:
	rm -f $(TARGETDIR)/*
