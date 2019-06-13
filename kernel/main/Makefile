CC= gcc -m32 -fno-stack-protector
LIB=/home/twchang/microos/uos/kernel/print/print.o

kernel: main.o
	ld -m elf_i386 main.o $(LIB) -Ttext 0xc0001500 -e main -o kernel

main.o: main.c
	$(CC) -c main.c -o main.o

.PHONY: clean

clean:
	rm kernel

.PHONY: cleano

cleano:
	rm *.o

.PHONY: dd

dd:
	dd if=./kernel of=/home/twchang/microos/bochs/64M.img bs=512 seek=9 count=200 conv=notrunc
