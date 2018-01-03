.PHONY: clean stats test

CC ?= gcc
CFLAGS += -Wall -pedantic -O2 -g

all: clean micro-lisp mlisp89 stats test

micro-lisp: micro-lisp.c
	$(CC) $(CFLAGS) -o $@ $^

mlisp89: mlisp89.c
	$(CC) $(CFLAGS) -ansi -std=c89 -D_DEFAULT_SOURCE -o $@ $^

stats: micro-lisp.c
	wc $^

test: micro-lisp mlisp89
	./test.sh ./micro-lisp
	./test.sh ./mlisp89

clean:
	@rm -fv micro-lisp mlisp89
