((lambda (exp)
   ((lambda (exp1)
      ((lambda (fn)
         (cond
           ((pair? fn)
              (write (quote (cons fn (cdr exp1)))))
           ((quote t)
              ((lambda (args)
                ((lambda (evlist)
                    (write (cons fn (evlist evlist args (quote ())))))
                  (lambda (evlist^ e1 evargs)
                    (cond
                      ((null? e1)  (quote ()))
                      ((quote t) (cons (car e1)
                                    (evlist^ evlist^ (cdr e1) evargs)))))))
              (cdr exp1)))))
       (car exp1)))
    (cond
      ((eq? (car exp) (quote apply))  (cdr exp))
      ((quote t) exp))))

  (quote (apply fn (1 2 3 4)))

 )
