Objective: implement a small Lisp/Scheme language in as few lines of C as
possible.

The interpreter supports `lambda`, e.g.

```lisp
  ((lambda (x) (cons x (quote 1))) (quote 7))
  (7 1)
```

And the special forms `if` and `quote`

```lisp
  (if (quote t) (quote 7) (quote 0))
```

The only types are tokens (non-interned strings containing no whitespace)
and pairs (cons cells).

Non-quoted tokens are looked up in the environment. If they have no associated
value the result is `null`. Because there is no numeric type a number for
example 7 will be treated as any other token and looked up in the environment.
Note in the examples above numbers are quoted to prevent that.

The built-in primitives are in the environment: `car`, `cdr`, `cons`, `eq`,
`pair?`.

