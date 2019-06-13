object:= $(patsubst %.s, %.bin, $(wildcard *.s))

all: $(object)

%.bin: %.s
	nasm -f bin $^ -o $@ 

.PHONY: dd

dd:
	dd if=./loader7.bin of=/home/twchang/microos/bochs/64M.img bs=512 seek=2 count=4 conv=notrunc

.PHONY: clean

clean:
	rm *.bin
