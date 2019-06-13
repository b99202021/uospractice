Dir:= /home/twchang/microos/uos/kernel

CC:= gcc

eflags:= -c -m32 -fno-stack-protector -fno-builtin -I $(Dir)/int -I . -I $(Dir)/thread -I $(Dir)/print -I $(Dir)/lib/list -I $(Dir)/bitmap 

timer.o: timer.c
	$(CC) $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o
