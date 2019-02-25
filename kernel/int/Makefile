CC=gcc

build_dir=/home/twchang/microos/uos/kernel

eflags= -m32 -c -I . -I ../print -fno-builtin -fno-stack-protector

all: int.o

object1:=$(patsubst %.s, %.o, $(wildcard *.s))
object2:=$(patsubst %.c, %.o, $(wildcard *.c))

object:= $(object1) $(object2)

int.o: $(object)
	ld -m elf_i386 -r $^ -o $@

$(object1): %.o: %.s
	nasm -f elf $< -o $@

$(object2): %.o: %.c
	$(CC) $(eflags) $< -o $@

.PHONY: clean

clean:
	rm *.o
