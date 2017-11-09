target exec ./micro-lisp
#break micro-lisp.c:500
#break micro-lisp.c:179
#break gettoken
break main
run < repl.lisp
