((lambda (exp)
  (cond
      ((eq? (car exp) (quote apply))
          (cons (car (cdr exp)) (car (cdr (cdr exp)))))
      ((quote t) exp)))

 (quote (apply fn (1 2 3 4))))
