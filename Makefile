.PHONY: clean stats test

CC ?= gcc
CFLAGS += -Wall -pedantic -O2 -g

all: clean mlisp89 stats test

non-bsd: clean mlisp89-non-bsd stats test

micro-lisp: micro-lisp.c
	$(CC) $(CFLAGS) -o $@ $^

mlisp89: mlisp89.c
	$(CC) $(CFLAGS) -ansi -std=c89 -D_DEFAULT_SOURCE -o $@ $^

mlisp89-non-bsd: mlisp89.c
	$(CC) $(CFLAGS) -ansi -lbsd -std=c89 -D_DEFAULT_SOURCE -o mlisp89 $^

stats: mlisp89.c
	wc $^

test: mlisp89
	./test.sh ./mlisp89

clean:
	@rm -fv micro-lisp mlisp89
