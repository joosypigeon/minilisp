;; equal? test cases

;; Numbers
(print (if (equal? 42 42) "pass: int equal" "fail: int equal"))
(print (if (equal? 42 43) "fail: int unequal" "pass: int unequal"))

;; Symbols
(print (if (equal? 'x 'x) "pass: symbol equal" "fail: symbol equal"))
(print (if (equal? 'x 'y) "fail: symbol unequal" "pass: symbol unequal"))

;; Strings
(print (if (equal? "hello" "hello") "pass: string equal" "fail: string equal"))
(print (if (equal? "hello" "world") "fail: string unequal" "pass: string unequal"))

;; NIL comparisons
(print (if (equal? NIL NIL) "pass: nil equal" "fail: nil equal"))
(print (if (equal? NIL 'x) "fail: nil unequal" "pass: nil unequal"))

;; Lists
(print (if (equal? '(1 2 3) '(1 2 3)) "pass: list equal" "fail: list equal"))
(print (if (equal? '(1 2 3) '(1 2 4)) "fail: list unequal" "pass: list unequal"))
(print (if (equal? '(1 (2 3)) '(1 (2 3))) "pass: nested list equal" "fail: nested list equal"))
(print (if (equal? '(1 (2 3)) '(1 (2 4))) "fail: nested list unequal" "pass: nested list unequal"))

;; Lambdas
(print (if (equal? (lambda (x) x) (lambda (x) x))
         "fail: lambda structural equality" 
         "pass: lambda not structurally equal"))

;; Quoted vs unquoted
(define q1 '(a b c))
(define q2 '(a b c))
(print (if (equal? q1 q2) "pass: quoted list equal" "fail: quoted list equal"))
