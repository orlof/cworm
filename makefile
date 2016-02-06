CC=gcc
CFLAGS=-I. -DDEBUG=1 -Wall
DEPS = array.h array_test.h interpreter.h memory.h memory_test.h minunit.h
OBJ = array.o array_test.o interpreter.o memory.o memory_test.o minunit.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testrun: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)