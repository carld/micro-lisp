(let ((f (macro (lambda (x)
                   (cond ((eq? x (quote foo)) (quote (quote bar)))
                         ((eq? x (quote baz)) (quote (quote qux))))))))
  (cons (f foo) (cons (f baz) (quote ()))))
