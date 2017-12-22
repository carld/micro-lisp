((lambda (e1 env1)
  ((lambda (assq)
      (assq assq e1 env1))
   (lambda (assq^ e1 env1)
      (cond
        ((null? env1) null)
        ((eq? e1 (car (car env1)))
            (car (cdr (car env1))))
        ((quote t)  (assq^ assq^ e1 (cdr env1)))))))

     (quote micro)

     (quote ((hello world)
             (world hello)
             (micro lisp))))
