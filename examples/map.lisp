((lambda (map)
    (map map
        (lambda (x)
          (cons x (cons (quote 0) null)))
        (cons (quote 1)
              (cons (quote 2)
                    (cons (quote 3) null)))))

  (lambda (map^ fn rest)
    (cond
     ((eq? null rest) rest)
     ((quote t)
        (cons (fn (car rest)) (map^ map^ fn (cdr rest))))) ))

