(((lambda (fn)
   ((lambda (h) (h h))
    (lambda (g)
      (fn (lambda (x)
            ((g g) x))))))

 (lambda (f)
   (lambda (lst)
     (if (null? lst)
       null
       (if (eq? (car lst) (quote a))
           (cons (quote 1) (f (cdr lst)))
           (if (eq? (car lst) (quote b))
             (cons (quote 2) (f (cdr lst)))
             (if (eq? (car lst) (quote c))
               (cons (quote 3) (f (cdr lst)))
               (cons (car lst) (f (cdr lst))))))))))

  (cons (quote a) (cons (quote b) (cons (quote c) null))))
