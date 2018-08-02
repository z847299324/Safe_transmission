CC = gcc 
CFLAGS = -Wall -c 
LDFLAGS = -lpthread
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
TARGET = main

$(TARGET):$(OBJS)
	    $(CC) $(LDFLAGS) -o $@ $^
%.o:%.c
	    $(CC) $(CFLAGS) -o $@ $<
.PHONY:clean
	clean:
	    @rm -f *.o $(TARGET)


