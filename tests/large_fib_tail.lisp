(define fib
  (lambda (n)
    (define iter
      (lambda (a b count)
        (if (= count 0)
            a
            (iter b (+ a b) (- count 1)))))
    (iter 0 1 n)))

(print (fib 15000))