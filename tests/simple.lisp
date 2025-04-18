;; Literal integer
(equal? 42 42)

;; Define a symbol and reference it
(define x 10)
(equal? x 10)

;; Define a simple lambda
(define inc (lambda (n) (+ n 1)))
(equal? (inc 5) 6)

;; Define a lambda with multiple parameters
(define add (lambda (a b) (+ a b)))
(equal? (add 3 4) 7)

;; Nested lambda (closure)
(define make-adder
  (lambda (x)
    (lambda (y) (+ x y))))
(define add5 (make-adder 5))
(equal? (add5 3) 8)

;; Shadowing and closure scope
(define outer 100)
(define shadow
  (lambda (outer) (lambda () outer)))
(equal? ((shadow 42)) 42)

;; Evaluate a lambda directly
(equal? ((lambda (x) (* x x)) 6) 36)

;; A lambda that returns another lambda
(equal? ((lambda (x) (lambda (y) (* x y))) 3) (lambda (y) (* 3 y)))

;; Apply the returned lambda
(define mul3 ((lambda (x) (lambda (y) (* x y))) 3))
(equal? (mul3 7) 21)

;; Redefine a symbol
(define x 99)
(equal? x 99)

(quote x)
;; Quote a symbol
(equal? (quote x) 'x)
