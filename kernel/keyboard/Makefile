
DIR = /home/twchang/microos/uos/kernel

# DIR := ..

eflags = -c -m32 -fno-builtin -fno-stack-protector -I . -I $(DIR)/print -I $(DIR)/int -I $(DIR)/ioqueue -I $(DIR)/thread -I $(DIR)/lib/list -I $(Dir)/bitmap

keyboard.o: keyboard.c
	gcc $(eflags) $^ -o $@

.PHONY : clean

clean:
	rm *.o
	
