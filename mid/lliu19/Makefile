CFLAGS=-c -g -Wall -pedantic-errors -std=c99 -D_BSD_SOURCE 

all: ls

ls: ls.o print.o cmp.o
	gcc -o ls ls.o print.o cmp.o -lbsd
ls.o: ls.c ls.h cmp.h
	gcc ${CFLAGS} ls.c
cmp.o: cmp.c cmp.h ls.h
	gcc ${CFLAGS} cmp.c
print.o: print.c print.h ls.h
	gcc ${CFLAGS} print.c
clean:
	rm *.o ls
