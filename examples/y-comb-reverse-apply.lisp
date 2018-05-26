(let ((Y (lambda (fn)
            ((lambda (h) (h h))
               (lambda (g)
                 (fn (lambda (x . args)
                    (apply (g g) (cons x args)))))))))

   (let ((reverse (lambda(list-input)
                    ((Y (lambda(reverse-recursive)
                       (lambda (a l)
                         (cond ((null? l) a)
                               ((quote t) (reverse-recursive (cons (car l) a) (cdr l))))))) (quote ()) list-input))))

      (reverse (quote (1 2 3 4 5 6 7 8 9)))))
