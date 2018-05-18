(let ((U (lambda (f) (f f))))
  ((U (lambda (f)
       (lambda (a l)
         (cond ((null? l) a)
               ((quote t) ((U f) (cons (car l) a) (cdr l)))))))
   null
   (quote (1 2 3 4 5 6 7 8 9))
  ))
