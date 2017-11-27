.PHONY: clean stats test

CC ?= gcc
CFLAGS += -Wall -pedantic -O2 -g

all: clean micro-lisp stats test

micro-lisp: micro-lisp.c
	$(CC) $(CFLAGS) -o $@ $^

stats: micro-lisp.c
	wc $^

test: micro-lisp
	./test.sh ./micro-lisp

clean:
	@rm -fv micro-lisp
