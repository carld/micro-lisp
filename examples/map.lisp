((lambda (map)
    (map map
        (lambda (x)
          (cons x (cons (quote 0) (quote ()))))
        (cons (quote 1)
              (cons (quote 2)
                    (cons (quote 3) (quote ()))))))

  (lambda (map^ fn rest)
    (cond
     ((eq? (quote ()) rest) rest)
     ((quote t)
        (cons (fn (car rest)) (map^ map^ fn (cdr rest))))) ))

