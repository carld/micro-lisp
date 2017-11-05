((lambda (e1 env1)
  ((lambda (assq)
      (assq assq e1 env1))
   (lambda (assq^ e1 env1)
      (if (null? env1)
         null
         (if (eq? e1 (car (car env1)))
             (car env1)
             (assq^ assq^ e1 (cdr env1)))))))

     (quote micro)

     (quote ((hello world)
             (world hello)
             (micro lisp))))
