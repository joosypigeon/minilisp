;; Create a variable in the global environment
(define a 10)

;; Define a function that captures its environment (closure)
(define make-fn
  (lambda (x)
    (lambda (y) (+ x y))))

;; Create a closure with x = 5
(define add5 (make-fn 5))

;; Shadow the global `a` in local call context
(define test
  (lambda (a)
    ;; argument `a` is evaluated in the calling environment,
    ;; but `add5` uses the environment from its definition
    (print (add5 a))))

;; Run test with argument from calling closure
(test a) ;; should print 15 (5 + 10)

;; Mutate global `a`
(set! a 20)
(test a) ;; should now print 25 (5 + 20)

;; Now test what happens if we rebind `add5` using current value of `a`
(define addA (make-fn a)) ;; captures x = 20 now
(print (addA 3)) ;; should print 23
