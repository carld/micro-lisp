#!/usr/bin/env bash

LISP=./micro-lisp

function check-ok {
  echo "$1 -> $2"
  echo "$1" | $LISP | grep -q "$2" && echo "Test OK" || echo "Test FAIL"
}

check-ok "(car (quote (1 2 3 4)))" "1"
check-ok "(cdr (quote (1 2 3 4)))" "(2 3 4)"
check-ok "(cons (quote 1) (quote 2))" "(1 2)"
check-ok "((lambda (x) (cons x (quote 1))) (quote 7))" "(7 1)"
check-ok "(eq (quote hello) (quote hello))" "(quote t)"
check-ok "(eq (quote hello) (quote world))" "null"
check-ok "(pair? (cons (quote hello) (quote world)))" "(quote t)"
check-ok "(pair? (quote hello))" "null"
check-ok "((1 (x) (cons x (quote 1))) 2)" "cannot evaluate expression"
check-ok "1" "null"
