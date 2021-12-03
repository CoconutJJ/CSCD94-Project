CC=clang -g -Wall

all: lox.c environment.o hashtable.o interpreter.o scanner.o parser.o error.o loxfunction.o resolver.o return.o
	$(CC) lox.c environment.o hashtable.o interpreter.o scanner.o parser.o error.o loxfunction.o resolver.o return.o -o lox

clean:
	rm *.o

return.o: return.c
	$(CC) -c return.c -o return.o

resolver.o: resolver.c
	$(CC) -c resolver.c -o resolver.o
loxfunction.o: loxfunction.c
	$(CC) -c loxfunction.c -o loxfunction.o

environment.o: environment.c  
	$(CC) -c environment.c -o environment.o

hashtable.o: hashtable/hash_table.c
	$(CC) -c hashtable/hash_table.c -o hashtable.o

interpreter.o: interpreter.c
	$(CC) -c interpreter.c -o interpreter.o

scanner.o: scanner.c
	$(CC) -c scanner.c -o scanner.o

parser.o: parser.c
	$(CC) -c parser.c -o parser.o

error.o: error.c
	$(CC) -c error.c -o error.o

