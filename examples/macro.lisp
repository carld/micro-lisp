(let ((f (macro (lambda ()
                  (cons (quote 1) (cons (quote 2) null))))))
   (f))
