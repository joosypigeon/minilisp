;; Literal integer
(print (if (equal? 42 42) "pass: literal integer" "fail: literal integer"))

;; Define a symbol and reference it
(define x 10)
(print (if (equal? x 10) "pass: symbol define" "fail: symbol define"))

;; Define a simple lambda
(define inc (lambda (n) (+ n 1)))
(print (if (equal? (inc 5) 6) "pass: inc" "fail: inc"))

;; Define a lambda with multiple parameters
(define add (lambda (a b) (+ a b)))
(print (if (equal? (add 3 4) 7) "pass: add" "fail: add"))

;; Nested lambda (closure)
(define make-adder
  (lambda (x)
    (lambda (y) (+ x y))))
(define add5 (make-adder 5))
(print (if (equal? (add5 3) 8) "pass: closure" "fail: closure"))

;; Shadowing and closure scope
(define outer 100)
(define shadow
  (lambda (outer) (lambda () outer)))
(print (if (equal? ((shadow 42)) 42) "pass: shadow" "fail: shadow"))

;; Evaluate a lambda directly
(print (if (equal? ((lambda (x) (* x x)) 6) 36) "pass: direct lambda" "fail: direct lambda"))

;; A lambda that returns another lambda
(print (if (equal? ((lambda (x) (lambda (y) (* x y))) 3) (lambda (y) (* 3 y)))
  "pass: lambda returns lambda"
  "fail: lambda returns lambda"))

;; Apply the returned lambda
(define mul3 ((lambda (x) (lambda (y) (* x y))) 3))
(print (if (equal? (mul3 7) 21) "pass: apply returned lambda" "fail: apply returned lambda"))

;; Redefine a symbol
(define x 99)
(print (if (equal? x 99) "pass: redefinition" "fail: redefinition"))

;; Quote test
(print (if (equal? (quote x) 'x) "pass: quote symbol" "fail: quote symbol"))

;; Define quoted list for iteration
(define quoted-list '(a (b c) d))

;; Recursive print over quoted list
(define loop-print
  (lambda (xs)
    (if (equal? xs NIL)
        NIL
        (begin
          (print (car xs))
          (loop-print (cdr xs))))))

;; Call the loop-print on the quoted list
(loop-print quoted-list)
