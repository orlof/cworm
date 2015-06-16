CC=gcc
CFLAGS=-I. -DDEBUG=1
DEPS = memory.h minunit.h array.h interpreter.h
OBJ = memory.o memory_test.o array.o interpreter.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testrun: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
