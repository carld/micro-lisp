((lambda (reverse)
    (reverse (quote (1 2 3 4 5 6 7 8 9))))

 (lambda (list)
   ((lambda (rev)
      (rev rev null list))
    (lambda (rev^ a l)
      (cond
        ((null? l) a)
        ((quote t) (rev^ rev^ (cons (car l) a) (cdr l ))))))))
