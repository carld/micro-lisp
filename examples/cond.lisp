(cond
   ((eq? (quote a) (quote b))  (quote c))
   ((eq? (quote a) (quote d))  (quote d))
   ((quote #t)                 (quote e)))
