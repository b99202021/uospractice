build_dir= /home/twchang/microos/uos/kernel

CC = gcc 

eflags = -m32 -fno-stack-protector -fno-builtin -I $(build_dir)/print -I $(build_dir)/int -I $(build_dir)/bitmap

LIB = $(build_dir)/print/print.o $(build_dir)/int/int.o $(build_dir)/bitmap/bitmap.o $(build_dir)/lib/string/string.o

kernel: main.o
	ld -m elf_i386 main.o $(LIB) -Ttext 0xc0001500 -e main -o kernel

main.o: main.c
	$(CC) $(eflags) -c main.c -o main.o

.PHONY: clean

clean:
	rm kernel *.o

.PHONY: dd

dd:
	dd if=./kernel of=/home/twchang/microos/bochs/64M.img bs=512 seek=9 count=200 conv=notrunc
