
build_dir = /home/twchang/microos/uos/kernel

eflags = -c -m32 -fno-stack-protector -fno-builtin -I $(build_dir)/print -I $(build_dir)/int -I $(build_dir)/bitmap -I $(build_dir)/thread -I $(build_dir)/timer -I $(build_dir)/lib/list \
-I $(build_dir)/console -I $(build_dir)/keyboard

ioqueue.o: ioqueue.c
	gcc $(eflags) $^ -o $@

.PHONY: clean

clean:
	rm *.o












