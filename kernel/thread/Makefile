Dir = /home/twchang/microos/uos/kernel

LIB = /home/twchang/microos/uos/kernel/lib

eflags = -m32 -c -I . -I $(Dir)/int -I $(Dir)/print -I $(Dir)/bitmap -I $(Dir)/usrprog -I $(LIB)/string -I $(LIB)/list -fno-builtin -fno-stack-protector

all: thread.o

thread.o: thread switch.o SYNC.o
	ld -m elf_i386 -r $^ -o $@

SYNC.o: SYNC.c
	gcc $(eflags) $^ -o $@

switch.o: switch.s
	nasm -f elf $< -o $@

thread: thread.c
	gcc $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o








