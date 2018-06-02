#!/usr/bin/env bash

LISP="${1:-./mlisp89}"
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

check-file-ok() {
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
check-ok '(cons (quote 1) (cons (quote 2) (quote ())))' '(1 2)'
check-ok '((lambda (x) (cons x (cons (quote 1) (quote ())))) (quote 7))' '(7 1)'
check-ok '((lambda (x) (cons x (quote 1))) (quote 7))' '(7 . 1)'
check-ok '(pair? (quote (1 2 3)))' '#t'
check-ok '(eq? (quote hello) (quote hello))' '#t'
check-ok '(eq? (quote hello) (quote world))' '()'
check-ok '(pair? (cons (quote hello) (quote world)))' '#t'
check-ok '(pair? (quote hello))' '()'
check-ok '(quote ())' '()'
check-ok '((lambda (x) (write x)) (quote hello))' 'hello'
check-ok '(write (quote (cons (quote 1) (quote 2))))' '(cons (quote 1) (quote 2))'
check-ok '(cons (quote 1) (cons (quote 2) (cons (quote 3) (cons (quote 4) null))))' '(1 2 3 4)'
check-ok '(quote (1 2 3 4))' '(1 2 3 4)'
check-ok '(cons (quote 1) (cons (quote 2) (quote ())))' '(1 2)'
check-ok '(write (cons (quote (hello world)) (quote ())))' '((hello world))'
check-ok '((lambda (x y) (cons y (cons x (quote ())))) (quote 67) (quote 89))' '(89 67)'
check-ok '(quote a-symbol-that-is-longer-than-thirty-two-bytes)' 'a-symbol-that-is-longer-than-th'
check-ok '((lambda (f x) (f)) ((lambda (x) (lambda () x)) (quote x)) (quote y))' 'x'
check-ok '(cond ((quote t)  (quote ok)))'  'ok'
check-ok '(cond ((quote ())  (quote ok)))'  '()'
check-ok '"hello world"' '"hello world"'
check-ok '1234' '1234'
check-ok '(+ 1 2)' '3'
check-ok '(- 10 3 2 1)' '4'
check-file-ok examples/assoc.lisp 'lisp'
check-file-ok examples/apply.lisp '(a b c)'
check-file-ok examples/apply2.lisp '(fn (1 2 3 4))'
check-file-ok examples/reverse.lisp '(9 8 7 6 5 4 3 2 1)'
check-file-ok examples/map.lisp '((1 0) (2 0) (3 0))'
check-file-ok examples/list.lisp '(a b c)'
check-file-ok examples/lambda-logic.lisp '(true false true false)'
check-file-ok examples/ycomb.lisp '(1 2 3)'
check-file-ok examples/u-comb.lisp '(9 8 7 6 5 4 3 2 1)'
check-file-ok examples/u-comb-reverse.lisp '(9 8 7 6 5 4 3 2 1)'
check-file-ok examples/y-comb-reverse.lisp '(9 8 7 6 5 4 3 2 1)'
check-file-ok examples/y-comb-reverse-apply.lisp '(9 8 7 6 5 4 3 2 1)'
check-file-ok examples/macro.lisp 'this'
check-file-ok examples/macro-ex.lisp '(bar qux)'
check-file-ok examples/lambda-rest.lisp '2'
check-file-ok examples/append.lisp '(1 2 3 4 5 6)'
check-file-ok examples/curry.lisp '(HEAD TAIL)'
check-file-ok examples/quasiquote.lisp '(a b e d)'
check-file-ok examples/cond.lisp 'e'
check-file-ok examples/built-in-map.lisp '(2 3 4)'
check-file-ok examples/let.lisp '3'
check-file-ok examples/format.lisp 'hello 21553 test world
#t'

echo "Passed $SUCCESS of $TOTAL"

# Meta Circular Interpreter Tests

#check-repl-ok 'hello' 'carl'
#check-repl-ok '(quote (write hello))' '(write hello)'
#check-repl-ok '(write (cons (quote hello) (cons (quote world) null)))' '(hello world)'
#check-repl-ok '(apply write (cons (cons (quote hello) (cons (quote world) null)) null))' '(hello world)'
#
#check-repl-file-ok examples/eval.lisp 'hello'
#check-repl-file-ok examples/assoc.lisp 'lisp'
#check-repl-file-ok examples/apply.lisp '(fn 1 2 3 4)'
#check-repl-file-ok examples/apply2.lisp '(fn (1 2 3 4))'
#check-repl-file-ok examples/reverse.lisp '(9 8 7 6 5 4 3 2 1)'
#check-repl-file-ok examples/map.lisp '((1 0) (2 0) (3 0))'
#check-repl-file-ok examples/lambda-logic.lisp '(true false true false)'
#check-repl-file-ok examples/ycomb.lisp '(1 2 3)'

#echo "Passed $SUCCESS of $TOTAL"

