
CC=gcc -g -Wall
CFLAGS=

all: clox

clean:
	\rm *.o

clox: main.c chunk.o compiler.o debug.o memory.o object.o scanner.o table.o value.o vm.o
	$(CC) $(CFLAGS) chunk.o compiler.o debug.o memory.o object.o scanner.o table.o value.o vm.o main.c -o clox

chunk.o: chunk.c chunk.h
	$(CC) $(CFLAGS) -c chunk.c -o chunk.o

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c -o compiler.o

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c -o debug.o

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c -o memory.o

object.o: object.c object.h
	$(CC) $(CFLAGS) -c object.c -o object.o

scanner.o: scanner.c scanner.h
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

table.o: table.c table.h
	$(CC) $(CFLAGS) -c table.c -o table.o

value.o: value.c value.h
	$(CC) $(CFLAGS) -c value.c -o value.o

vm.o: vm.c vm.h
	$(CC) $(CFLAGS) -c vm.c -o vm.o