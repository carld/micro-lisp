#!/usr/bin/env bash

LISP=./micro-lisp
TOTAL=0
SUCCESS=0

function check-ok {
  echo "$1 -> $2"
#  echo $1 | $LISP
  echo "$1" | $LISP | grep -q "^$2$"
  if [ $? -eq 0 ]; then
    echo "Test OK"
    SUCCESS=$((SUCCESS + 1))
  else
    echo "Test FAIL"
  fi
  TOTAL=$((TOTAL + 1))
}

function check-prog-ok {
  echo "$1 -> $2"
#  echo $1 | $LISP
  cat $1 | $LISP | grep -q "^$2$"
  if [ $? -eq 0 ]; then
    echo "Test OK"
    SUCCESS=$((SUCCESS + 1))
  else
    echo "Test FAIL"
  fi
  TOTAL=$((TOTAL + 1))
}

function check-repl-ok {
  echo "$1 -> $2"
  echo $1 | cat repl0.lisp - | ./micro-lisp | grep -q "^$2$"
  if [ $? -eq 0 ]; then
    echo "Test OK"
    SUCCESS=$((SUCCESS + 1))
  else
    echo "Test FAIL"
  fi
  TOTAL=$((TOTAL + 1))
}

check-ok "(car (quote (1 2 3 4)))" "1"
check-ok "(cdr (quote (1 2 3 4)))" "(2 3 4)"
check-ok "(cons (quote 1) (cons (quote 2) null))" "(1 2)"
check-ok "((lambda (x) (cons x (cons (quote 1) null))) (quote 7))" "(7 1)"
check-ok "(pair? (quote (1 2 3)))" "(quote t)"
check-ok "(eq? (quote hello) (quote hello))" "(quote t)"
check-ok "(eq? (quote hello) (quote world))" "null"
check-ok "(pair? (cons (quote hello) (quote world)))" "(quote t)"
check-ok "(pair? (quote hello))" "null"
check-ok "((1 (x) (cons x (quote 1))) 2)" "null"
check-ok "1" "null"
check-ok "((lambda (x) (write x)) (quote hello))" "hello"
check-ok "(write (quote (cons (quote 1) (quote 2))))" "(cons (quote 1) (quote 2))"
check-ok "(cons (quote 1) (cons (quote 2) (cons (quote 3) (cons (quote 4) null))))" "(1 2 3 4)"
check-ok "(quote (1 2 3 4))" "(1 2 3 4)"
check-ok "(cons (quote 1) (cons (quote 2) null))" "(1 2)"
check-ok "(apply write (cons (quote (hello world)) null))" "(hello world)"
check-ok "((lambda (x y) (cons y (cons x null))) (quote 67) (quote 89))" "(89 67)"

check-prog-ok assoc.lisp "lisp"
check-prog-ok apply.lisp "(fn 1 2 3 4)"
check-prog-ok apply2.lisp "(fn (1 2 3 4))"

check-repl-ok "hello" "carl"
check-repl-ok "(quote (write hello))" "(write hello)"

echo "Passed $SUCCESS of $TOTAL"

