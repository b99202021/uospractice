CC= gcc 
eflags=-m32 -fno-stack-protector -fno-builtin -I /home/twchang/microos/uos/kernel/print -I /home/twchang/microos/uos/kernel/int
LIB=/home/twchang/microos/uos/kernel/print/print.o /home/twchang/microos/uos/kernel/int/int.o

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
