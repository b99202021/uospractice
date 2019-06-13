CC = gcc

Dir := /home/twchang/microos/uos/kernel

eflags = -m32 -I . -I $(Dir)/print -fno-builtin -fno-stack-protector

all : syscall.o

syscall.o: syscall.c
	$(CC) $(eflags) -c $^ -o $@

.PHONY : clean

clean:
	rm *.o
