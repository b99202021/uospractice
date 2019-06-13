build_dir= /home/twchang/microos/uos/kernel

CC = gcc 

eflags = -m32 -fno-stack-protector -fno-builtin -I $(build_dir)/print -I $(build_dir)/int -I $(build_dir)/bitmap -I $(build_dir)/thread -I $(build_dir)/timer -I $(build_dir)/lib/list \
-I $(build_dir)/console -I $(build_dir)/keyboard -I $(build_dir)/ioqueue -I $(build_dir)/usrprog

INC = $(build_dir)/print/print.o $(build_dir)/int/int.o $(build_dir)/bitmap/bitmap.o $(build_dir)/lib/string/string.o $(build_dir)/thread/thread.o \
$(build_dir)/lib/list/list.o $(build_dir)/timer/timer.o $(build_dir)/console/console.o $(build_dir)/keyboard/keyboard.o $(build_dir)/ioqueue/ioqueue.o \
$(build_dir)/usrprog/usrprog.o

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
