;; Basic let with one binding
(print (if (equal? 
             (let ((x 42)) x)
             42)
           "pass: let one binding"
           "fail: let one binding"))

;; Let with multiple bindings
(print (if (equal? 
             (let ((a 1) (b 2)) (+ a b))
             3)
           "pass: let two bindings"
           "fail: let two bindings"))

;; Let with shadowing
(print (if (equal?
             (let ((x 1))
               (let ((x 2)) x))
             2)
           "pass: let shadowing inner wins"
           "fail: let shadowing inner wins"))

(print (if (equal?
             (let ((x 1))
               (let ((x 2)) (+ x x)))
             4)
           "pass: let shadowing with use"
           "fail: let shadowing with use"))

;; Let inside lambda
(define make-pair
  (lambda (a b)
    (let ((x a) (y b))
      (cons x y))))

(print (if (equal?
             (make-pair 7 8)
             (cons 7 8))
           "pass: let inside lambda"
           "fail: let inside lambda"))

;; Let used with computed values
(print (if (equal?
             (let ((x 3)
                   (y (* 2 2)))
               (+ x y))
             7)
           "pass: let with computed bindings"
           "fail: let with computed bindings"))

;; Let with list in body
(print (if (equal?
             (let ((xs '(1 2 3)))
               (car xs))
             1)
           "pass: let with list binding"
           "fail: let with list binding"))
