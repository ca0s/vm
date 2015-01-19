CC = gcc -Wall

all: doit
debug: CC += -g
debug: doit

doit: virtual.o opcodes.o stack.o interrupts.o
	$(CC) -o virtual.exe virtual.o opcodes.o stack.o interrupts.o
	
virtual.o: virtual.c virtual.h
	$(CC) -c virtual.c
opcodes.o: opcodes.c opcodes.h
	$(CC) -c opcodes.c
stack.o: stack.c stack.h
	$(CC) -c stack.c
interrupts.o: interrupts.c interrupts.h
	$(CC) -c interrupts.c
as.o: as.c assembler.h opcodes.h
	$(CC) -c as.c
assemble.o: assemble.c assembler.h opcodes.h
	$(CC) -c assemble.c

.PHONY=clean
clean:
	rm *.o virtual
