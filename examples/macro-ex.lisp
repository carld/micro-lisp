(let ((f (macro (lambda (x)
                   (cond ((eq? x (quote foo)) (quote bar))
                         ((eq? x (quote baz)) (quote qux)))))))
  (cons (f foo) (f baz)))
