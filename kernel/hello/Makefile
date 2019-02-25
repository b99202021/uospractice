hello: hello.o
	ld -m elf_i386 hello.o -Ttext 0xc0001500 -e main -o hello
hello.o: hello.c
	gcc -m32 -c -o hello.o hello.c 

.PHONY: dd

dd:
	dd if=./hello of=/home/twchang/microos/bochs/64M.img bs=512 seek=9 count=200 conv=notrunc

.PHONY: clean

clean:
	rm hello.o
	rm hello
