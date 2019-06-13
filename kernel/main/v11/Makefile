Dir= /home/twchang/microos/uos/kernel

CC = gcc 

eflags = -m32 -fno-stack-protector -fno-builtin -I $(Dir)/print -I $(Dir)/int -I $(Dir)/bitmap -I $(Dir)/thread -I $(Dir)/timer -I $(Dir)/lib/list \
-I $(Dir)/console -I $(Dir)/keyboard -I $(Dir)/ioqueue -I $(Dir)/usrprog -I $(Dir)/lib/syscall

INC = $(Dir)/print/print.o $(Dir)/int/int.o $(Dir)/bitmap/bitmap.o $(Dir)/lib/string/string.o $(Dir)/thread/thread.o \
$(Dir)/lib/list/list.o $(Dir)/timer/timer.o $(Dir)/console/console.o $(Dir)/keyboard/keyboard.o $(Dir)/ioqueue/ioqueue.o \
$(Dir)/usrprog/usrprog.o $(Dir)/lib/syscall/syscall.o

kernel: main.o
	ld -m elf_i386 main.o $(INC) -Ttext 0xc0001500 -e main -o kernel

main.o: main.c
	$(CC) $(eflags) -c main.c -o main.o

.PHONY: clean

clean:
	rm kernel *.o

.PHONY: dd

dd:
	dd if=./kernel of=/home/twchang/microos/bochs/64M.img bs=512 seek=9 count=200 conv=notrunc
