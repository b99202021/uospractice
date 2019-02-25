CC := gcc

DIR := /home/twchang/microos/uos/kernel

eflags := -c -m32 -fno-builtin -fno-stack-protector -I . -I $(DIR)/int -I $(DIR)/lib/list -I $(DIR)/print -I $(DIR)/thread -I $(DIR)/bitmap -I $(DIR)/lib/string -I $(DIR)/bitmap

usrprog.o: tss.o process.o
	ld -m elf_i386 -r $^ -o $@

tss.o: tss.c
	$(CC) $(eflags) $^ -o $@

process.o: process.c
	$(CC) $(eflags) $^ -o $@

.PHONY : clean

clean:
	rm *.o
