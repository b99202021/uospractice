#object:=$(patsubst %.c , %.o $(wildcard *.c))

all: bitmap.o

build_dir:= /home/twchang/microos/uos/kernel

eflags = -m32 -c -I . -I $(build_dir)/lib/list -I $(build_dir)/int -I $(build_dir)/print -I $(build_dir)/lib/string -I $(build_dir)/thread -fno-builtin -fno-stack-protector

CC=gcc

bitmap.o: bitmap.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
