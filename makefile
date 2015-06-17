CC=gcc
CFLAGS=-I. -DDEBUG=1 -Wall
DEPS = array.h interpreter.h memory.h minunit.h
OBJ = array.o interpreter.o memory.o minunit.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testrun: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)