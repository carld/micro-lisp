CC=gcc
CFLAGS= -Wall -pedantic -O2 -g
default: micro-lisp
	./test.sh

micro-lisp: micro-lisp.c
	$(CC) $(CFLAGS) -o $@ $^
	wc micro-lisp.c

clean:
	rm -v micro-lisp
