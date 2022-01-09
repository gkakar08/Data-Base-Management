all: a6

a6.o: a6.c hash.h
	clang a6.c -pedantic -Wall -std=c99 -c
a6: a6.o hash.o
	clang a6.o hash.o -pedantic -Wall -std=c99 -o a6

clean:
	rm *.o

