;; Edge case: deeply nested expressions
(print 
  (if 
    (equal? 
      ((((lambda (x) 
           (lambda (y) 
             (lambda (z) 
               (+ x (+ y z))))) 1) 2) 3)
      6)
    "pass: deep nest"
    "fail: deep nest"))