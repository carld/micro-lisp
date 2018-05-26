(let ((Y (lambda (fn)
            ((lambda (h) (h h))
               (lambda (g)
                 (fn (lambda (x . y)
                    (apply (g g) x y))))))))

   (let ((append (Y (lambda (recursive-append)
                       (lambda (l1 l2)
                         (cond ((null? l1) l2)
                               ((quote t) (cons (car l1)
                                                (recursive-append (cdr l1) l2)))))))))

      (let ((curry (lambda (fn . args)
                    (lambda (x)
                      (apply fn (append args x))))))

        (let ((cons-HEAD (curry cons (quote HEAD))))
          (cons-HEAD (quote ((TAIL))) )))))
