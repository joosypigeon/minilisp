;; Define a function with multiple expressions in its body
(define f1
  (lambda (x)
    (print "evaluating f1")
    (+ x 1)
    (+ x 2)
    (+ x 3)))  ; only this result is returned

(print (if (equal? (f1 5) 8) "pass: f1 return value" "fail: f1 return value"))

;; Nested functions and sequencing
(define make-counter
  (lambda ()
    (define count 0)
    (lambda ()
      (set! count (+ count 1))
      (print count)
      count)))

(define counter1 (make-counter))
(counter1)
(counter1)
(print (if (equal? (counter1) 3) "pass: closure count" "fail: closure count"))

;; Ensure side effects still run
(define f2
  (lambda (x)
    (print "start f2")
    (define y (+ x 10))
    (print y)
    (+ y 1)))

(print (if (equal? (f2 2) 13) "pass: f2 result" "fail: f2 result"))

;; Body with let and sequence
(define f3
  (lambda (a b)
    (let ((sum (+ a b)))
      (print sum)
      (* sum 2))))

(print (if (equal? (f3 2 3) 10) "pass: f3 let-sequence" "fail: f3 let-sequence"))

;; Recursive with internal print
(define factorial
  (lambda (n)
    (print n)
    (if (equal? n 0)
        1
        (* n (factorial (- n 1))))))

(print (if (equal? (factorial 4) 24) "pass: factorial" "fail: factorial"))

;; Closure that prints both env and return
(define make-printer
  (lambda (prefix)
    (lambda (msg)
      (print prefix)
      (print msg)
      (string-append prefix msg))))

(define hello (make-printer "Hello, "))
(print (if (equal? (hello "Simon") "Hello, Simon") "pass: hello closure" "fail: hello closure"))
