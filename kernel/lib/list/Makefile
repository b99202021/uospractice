Dir:= /home/twchang/microos/uos/kernel

CC:= gcc

eflags:= -c -m32 -fno-stack-protector -fno-builtin -I . -I $(Dir)/print -I $(Dir)/int

list.o: list.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
