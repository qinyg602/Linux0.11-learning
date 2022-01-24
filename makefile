AS=nasm
LD=ld
CC=gcc
CFLAG=-Wall -O -finline-functions -nostdinc -Iincludes -fno-stack-protector -g -c
PAGE_OFFSET=0xC0000000
Disk:Image
	sudo dd if=Image of=floppy.img
	#sudo dd if=Image of=/dev/fd0
Image:boot/boot.bin boot/setup.bin init/system tools/build
	tools/build boot/boot.bin boot/setup.bin init/system>Image
boot/boot.bin:boot/boot.asm
	nasm -f bin -o $@ $< 
boot/setup.bin:boot/setup.asm
	nasm -f bin -o $@ $<
boot/head.o:boot/head.asm
	nasm -f elf -o $@ $<
lib/lib.o:
	(cd lib;make)
kernel/kernel.o:
	(cd kernel;make)
mm/mm.o:
	(cd mm;make)
init/main.o:init/main.c
	$(CC) -o $@ $(CFLAG) $<
init/system:boot/head.o lib/lib.o init/main.o kernel/kernel.o mm/mm.o
	$(LD) -Ttext $(PAGE_OFFSET) -o $@ $^
tools/build:tools/build.c
	$(CC) -o $@ $<
.PHONY clean:
clean:
	-rm -f boot/*.bin boot/*.o init/system tools/build init/*.o Image
	(cd kernel;make clean;)
	(cd mm;make clean;)
	(cd lib;make clean;)
