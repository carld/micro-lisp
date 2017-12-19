#!/usr/bin/env bash

LISP="${1:-./micro-flisp}"
TOTAL=0
SUCCESS=0

success-counter() {
  if [ $? -eq 0 ]; then
    echo Test OK
    SUCCESS=$((SUCCESS + 1))
  else
    echo Test FAIL
  fi
  TOTAL=$((TOTAL + 1))
}

check-ok() {
  echo "$1 -> $2"
  echo "$1" | $LISP | grep -q "^$2$"
  success-counter
}

check-prog-ok() {
  echo "$1 -> $2"
  cat $1 | $LISP | grep -q "^$2$"
  success-counter
}

check-repl-ok() {
  echo "$1 -> $2"
  echo $1 | cat examples/repl0.lisp - | $LISP | grep -q "^$2$"
  success-counter
}

check-repl-file-ok() {
  echo "$1 -> $2"
  cat examples/repl0.lisp $1 | $LISP | grep -q "^$2$"
  success-counter
}

check-ok '(car (quote (1 2 3 4)))' '1'
check-ok '(cdr (quote (1 2 3 4)))' '(2 3 4)'
check-ok '(cons (quote 1) (cons (quote 2) null))' '(1 2)'
check-ok '((lambda (x) (cons x (cons (quote 1) null))) (quote 7))' '(7 1)'
check-ok '((lambda (x) (cons x (quote 1))) (quote 7))' '(7 . 1)'
check-ok '(pair? (quote (1 2 3)))' '(quote t)'
check-ok '(eq? (quote hello) (quote hello))' '(quote t)'
check-ok '(eq? (quote hello) (quote world))' 'null'
check-ok '(pair? (cons (quote hello) (quote world)))' '(quote t)'
check-ok '(pair? (quote hello))' 'null'
check-ok '((1 (x) (cons x (quote 1))) 2)' 'null'
check-ok '1' 'null'
check-ok '((lambda (x) (write x)) (quote hello))' 'hello'
check-ok '(write (quote (cons (quote 1) (quote 2))))' '(cons (quote 1) (quote 2))'
check-ok '(cons (quote 1) (cons (quote 2) (cons (quote 3) (cons (quote 4) null))))' '(1 2 3 4)'
check-ok '(quote (1 2 3 4))' '(1 2 3 4)'
check-ok '(cons (quote 1) (cons (quote 2) null))' '(1 2)'
check-ok '(write (cons (quote (hello world)) null))' '((hello world))'
check-ok '((lambda (x y) (cons y (cons x null))) (quote 67) (quote 89))' '(89 67)'
check-ok '(quote a-symbol-that-is-longer-than-thirty-two-bytes)' 'a-symbol-that-is-longer-than-th'
check-ok '((lambda (f x) (f)) ((lambda (x) (lambda () x)) (quote x)) (quote y))' 'x'

check-prog-ok examples/assoc.lisp 'lisp'
check-prog-ok examples/apply.lisp '(fn 1 2 3 4)'
check-prog-ok examples/apply2.lisp '(fn (1 2 3 4))'
check-prog-ok examples/reverse.lisp '(9 8 7 6 5 4 3 2 1)'
check-prog-ok examples/map.lisp '((1 0) (2 0) (3 0))'
check-prog-ok examples/lambda-logic.lisp '(true false true false)'

check-repl-ok 'hello' 'carl'
check-repl-ok '(quote (write hello))' '(write hello)'
check-repl-ok '(write (cons (quote hello) (cons (quote world) null)))' '(hello world)'
check-repl-ok '(apply write (cons (cons (quote hello) (cons (quote world) null)) null))' '(hello world)'

check-repl-file-ok examples/eval.lisp 'hello'

echo "Passed $SUCCESS of $TOTAL"

