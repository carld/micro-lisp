(let ((l (lambda (x . rest)
           (car rest))))
  (l (quote 1) (quote 2) (quote 3) (quote 4)))
