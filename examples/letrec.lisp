(letrec ((x (lambda (y r)
              (if (= y 0) r
                (cons y (x (- y 1) r))))))
  (x 4 (quote ())))
