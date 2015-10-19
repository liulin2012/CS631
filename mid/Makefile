# -Wall in CFLAGS
CFLAGS=-c -g -std=c99 -D_BSD_SOURCE -Wall -pedantic-errors 
# CFLAGS=-c -g -std=c99 -lbsd -Wall -pedantic-errors 
all: ls

ls: ls.o print.o cmp.o
	gcc -o ls ls.o print.o cmp.o 
ls.o: ls.c ls.h cmp.h
	gcc ${CFLAGS} ls.c
print.o: print.c print.h ls.h
	gcc ${CFLAGS} print.c
cmp.o: cmp.c cmp.h ls.h
	gcc ${CFLAGS} cmp.c
clean:
	rm *.o ls
