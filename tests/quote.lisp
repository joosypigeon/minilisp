(define quoted-list
  (list
    '(a (b c) d)
    'hello
    '(a b c)
    'a
    '(1 2 3)
    '((a b) c)))

(define loop-print
  (lambda (xs)
    (if (equal? xs NIL)
        NIL
        (begin
          (print (car xs))
          (loop-print (cdr xs))))))
          
(loop-print quoted-list)