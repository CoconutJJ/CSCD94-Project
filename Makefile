GCC=gcc -g -Wall

all: lox.c environment.o hashtable.o interpreter.o scanner.o parser.o error.o loxfunction.o resolver.o return.o
	$(GCC) lox.c environment.o hashtable.o interpreter.o scanner.o parser.o error.o loxfunction.o resolver.o return.o -o lox

clean:
	rm *.o

return.o: return.c
	$(GCC) -c return.c -o return.o

resolver.o: resolver.c
	$(GCC) -c resolver.c -o resolver.o
loxfunction.o: loxfunction.c
	$(GCC) -c loxfunction.c -o loxfunction.o

environment.o: environment.c  
	$(GCC) -c environment.c -o environment.o

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

