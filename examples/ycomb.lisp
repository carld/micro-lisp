(((lambda (fn)
   ((lambda (h) (h h))
    (lambda (g)
      (fn (lambda (x)
            ((g g) x))))))

 (lambda (f)
   (lambda (lst)
     (cond
       ((null? lst) (quote ()))
       ((eq? (car lst) (quote a)) (cons (quote 1) (f (cdr lst))))
       ((eq? (car lst) (quote b)) (cons (quote 2) (f (cdr lst))))
       ((eq? (car lst) (quote c)) (cons (quote 3) (f (cdr lst))))
       ((quote t) (cons (car lst) (f (cdr lst))))))))

  (cons (quote a) (cons (quote b) (cons (quote c) (quote ())))))
