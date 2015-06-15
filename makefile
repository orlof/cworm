CC=gcc
CFLAGS=-I.
DEPS = memory.h minunit.h
OBJ = memory.o memory_test.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testrun: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
