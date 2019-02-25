build_dir:= /home/twchang/microos/uos/kernel

CC:= gcc

eflags:= -c -m32 -fno-stack-protector -fno-builtin -I . -I $(build_dir)/print -I $(build_dir)/int

list.o: list.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
