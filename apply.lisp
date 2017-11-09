((lambda (exp)
  (if (eq? (car exp) (quote apply))
      (cons (car (cdr exp)) (car (cdr (cdr exp))))
      exp))

 (quote (apply fn (1 2 3 4))))
