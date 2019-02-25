build_dir:= /home/twchang/microos/uos/kernel

CC:= gcc

eflags:= -c -m32 -fno-stack-protector -fno-builtin -I $(build_dir)/int -I . -I $(build_dir)/thread -I $(build_dir)/print -I $(build_dir)/lib/list

timer.o: timer.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
