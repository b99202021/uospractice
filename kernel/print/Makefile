# using ld linker with -r can merge relocatable file 

CC=gcc -fno-stack-protector

object1:=$(patsubst %.s, %.o, $(wildcard *.s))
object2:=$(patsubst %.c, %.o, $(wildcard *.c))

objects:=$(object1) $(object2)

all: print.o

print.o: $(objects)
	ld -m elf_i386 -r $^ -o $@

$(object2): %.o: %.c
	$(CC) -c -m32 $< -o $@

$(object1): %.o: %.s
	nasm -f elf $< -o $@

.PHONY: clean

clean:
	rm -rf *.o

