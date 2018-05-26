### Micro Lisp

Objective: implement a small Lisp/Scheme language in as little C code as possible.

_This is a hobby project for educational purposes, it has bugs and may fail without warning. Pull requests and improvements welcome_

The interpreter supports `lambda`, e.g.

```lisp
  ((lambda (x) (cons x (quote 1))) (quote 7))
  (7 . 1)
```

Note that `lambda` does not capture free variables (variables that are not passed as arguments and refer to an outer scope). Free variables will resolve to their assigned values in the environment when the body of the lambda is evaluated.

The special forms `if` and `quote` behave in a typical way:

```lisp
  (if (quote t) (quote 7) (quote 0))
  7
```

The only types are symbols and pairs.

Non-quoted symbols are looked up in the environment. If they have no associated
value the result is `null`; in fact, zero. Because there is no numeric type a 
number e.g. `7` will be treated like any other symbol and looked up in the environment. 
Note in the examples above how numbers are quoted to prevent that.

The built-in primitives in the environment are: `car`, `cdr`, `cons`, `eq?`,
`pair?`, `read`, `write`.

Also provided is `apply` which takes a function and a single list argument:

```lisp
  (apply write (quote ((hello world))))
  (hello world)
  (quote t)
```

Lists can be built up by `cons`ing:

```lisp
  (apply write (cons (cons (quote hello) (cons (quote world) null)) null))
  (hello world)
  (quote t)
```

### Read Eval Print Loop

A REPL is implemented in micro-lisp itself. To try it out in a terminal:

```
  cat repl.lisp - | ./micro-lisp
```

To exit, press 'control c' to terminate the process.

Note the `-` argument to `cat` to pipe stdin through, otherwise micro-lisp will receive end-of-file.

The source code for the REPL is in `repl.lisp`. It implements `eval` and provides an environment which resolves symbols to the primitive functions in the underlying micro-lisp interpreter.

### Debugging with GDB

A `.gdbinit` file sets the target, breakpoints and runs the executable. Simply run `gdb`.


Pull requests welcome.

[![asciicast](https://asciinema.org/a/HbNmch4GVH9jXpeY3I6yvfqPF.png)](https://asciinema.org/a/HbNmch4GVH9jXpeY3I6yvfqPF?autoplay=1)

