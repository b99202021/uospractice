CC:= gcc

Dir := ../../

eflags := -m32 -fno-builtin -fno-stack-protector -I . -I $(Dir)/print -I $(Dir)/lib/string -I $(Dir)/usrprog -I $(Dir)/lib/syscall

stdio.o: stdio.c
	$(CC) -c $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o



















