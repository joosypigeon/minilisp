(define (null? x)
    (if (equal? x '())
        #t
        #f))

(define (lat l)
    (cond
        ((null? l) #t)
        ((atom? l) #t)
        ((atom? (car l)) (lat (cdr l)))
        (#t #f)))



(print (lat '(1 2 3))) ; => #t
(print (lat '(1 2 (3 4)))) ; => #f
(print (lat '((1 2) (3 4)))) ; => #f
(print (lat '((1 2) (3 4) (5 6)))) ; => #f
(print (lat '"hello")) ; => #t
(print (lat '())) ; => #t
(print (lat 'a)) ; => #t