AS=nasm
LD=ld
CC=gcc
CFLAG=-Wall -O -finline-functions -nostdinc -I../includes -fno-stack-protector -g -c
OBJECTS=klib.o klibasm.o
lib.o:$(OBJECTS)
	$(LD) -o $@ -r $(OBJECTS)
	sync
%.o:%.c
	$(CC) -o $@ $(CFLAG) $<
klibasm.o:klib.asm
	nasm -f elf -o $@ $<
.PHONY clean:
clean:
	-rm -f *.o
