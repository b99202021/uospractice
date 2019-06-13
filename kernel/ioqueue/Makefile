
Dir := /home/twchang/microos/uos/kernel

# Dir := ..

eflags = -c -m32 -fno-stack-protector -fno-builtin -I $(Dir)/print -I $(Dir)/int -I $(Dir)/bitmap -I $(Dir)/thread -I $(Dir)/timer -I $(Dir)/lib/list \
-I $(Dir)/console -I $(Dir)/keyboard

ioqueue.o: ioqueue.c
	gcc $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o












