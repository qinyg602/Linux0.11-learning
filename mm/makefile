AS=nasm
LD=ld
CC=gcc
CFLAG=-Wall -O -finline-functions -nostdinc -I../includes -fno-stack-protector -g -c
OBJECTS=memory.o page.o
mm.o:$(OBJECTS)
	$(LD) -o $@ -r $(OBJECTS)
	sync
%.o:%.c
	$(CC) -o $@ $(CFLAG) $<
%.o:%.asm
	nasm -f elf -o $@ $<
.PHONY clean:
clean:
	-rm *.o

