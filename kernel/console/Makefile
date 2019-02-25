
DIR = /home/twchang/microos/uos/kernel

eflags = -m32 -c -I . -I $(DIR)/thread -I $(DIR)/print -I $(DIR)/lib/list -I $(DIR)/int -fno-stack-protector -fno-builtin

console.o: console.c
	gcc $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o












