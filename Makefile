GCC=gcc -g -Wall

all: lox.c hashtable.o interpreter.o scanner.o parser.o error.o
	$(GCC) lox.c hashtable.o interpreter.o scanner.o parser.o error.o -o lox

hashtable.o: hashtable/hash_table.c
	$(GCC) -c hashtable/hash_table.c -o hashtable.o

interpreter.o: interpreter.c
	$(GCC) -c interpreter.c -o interpreter.o

scanner.o: scanner.c
	$(GCC) -c scanner.c -o scanner.o

parser.o: parser.c
	$(GCC) -c parser.c -o parser.o

error.o: error.c
	$(GCC) -c error.c -o error.o
