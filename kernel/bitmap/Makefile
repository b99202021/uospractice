#object:=$(patsubst %.c , %.o $(wildcard *.c))

all: bitmap.o

Dir:= /home/twchang/microos/uos/kernel

# Dir := ../

eflags = -m32 -c -I . -I $(Dir)/lib/list -I $(Dir)/int -I $(Dir)/print -I $(Dir)/lib/string -I $(Dir)/thread -fno-builtin -fno-stack-protector

CC=gcc

bitmap.o: bitmap.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
