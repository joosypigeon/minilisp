;; atom? should return true for a number
(print (if (atom? 42)
           "pass: atom? number"
           "fail: atom? number"))

;; atom? should return true for a symbol
(print (if (atom? 'x)
           "pass: atom? symbol"
           "fail: atom? symbol"))

;; atom? should return true for a string (if you support strings)
(print (if (atom? "hello")
           "pass: atom? string"
           "fail: atom? string"))

;; atom? should return true for a lambda
(define f (lambda (x) x))
(print (if (atom? f)
           "pass: atom? lambda"
           "fail: atom? lambda"))

;; atom? should return true for nil
(print (if (atom? ())
           "pass: atom? nil"
           "fail: atom? nil"))

;; atom? should return false for a pair (cons cell)
(print (if (atom? (cons 1 2))
           "fail: atom? cons"
           "pass: atom? cons"))

;; atom? should return false for a quoted list
(print (if (atom? '(a b c))
           "fail: atom? list"
           "pass: atom? list"))

;; atom? should return false for a nested list
(print (if (atom? '((a . 1) (b . 2)))
           "fail: atom? nested list"
           "pass: atom? nested list"))
