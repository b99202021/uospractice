CC := gcc

DIR := /home/twchang/microos/uos/kernel

eflags := -m32 -fno-builtin -fno-stack-protector -I . -I $(DIR)/int -I $(DIR)/lib/list -I $(DIR)/print -I $(DIR)/thread -I $(DIR)/bitmap -I $(DIR)/lib/string -I $(DIR)/bitmap -I $(DIR)/lib/string \
-I $(DIR)/console

usrprog.o: tss.o process.o syscall-init.o
	ld -m elf_i386 -r $^ -o $@

syscall-init.e: syscall-init.c
	$(CC) -E $(eflags) $^ -o $@

syscall-init.o: syscall-init.c
	$(CC) -c $(eflags) $^ -o $@

tss.o: tss.c
	$(CC) -c $(eflags) $^ -o $@

process.o: process.c
	$(CC) -c $(eflags) $^ -o $@

.PHONY : clean

clean:
	rm *.o
