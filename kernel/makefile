AS=nasm
LD=ld
CC=gcc
CFLAG=-Wall -O -finline-functions -nostdinc -I../includes -fno-stack-protector -g -c
#OBJECTS=sched.o trap.o asm.o system_call.o fork.o
OBJECTS:=$(patsubst %.c,%.o,$(wildcard *.c))
OBJECTS+=$(patsubst %.asm,%.o,$(wildcard *.asm))
kernel.o:$(OBJECTS)
	$(LD) -o $@ -r $(OBJECTS)
%.o:%.c
	$(CC) -o $@ $(CFLAG) $<
%.o:%.asm
	nasm -f elf -o $@ $< 
.PHONY clean:
clean:
	-rm -f *.o

