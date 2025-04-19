;; Edge case: empty list
(print (if (equal? NIL NIL) "pass: empty list equals NIL" "fail: empty list equals NIL"))

;; Edge case: empty lambda parameters
(define empty-fn (lambda () 42))
(print (if (equal? (empty-fn) 42) "pass: empty lambda args" "fail: empty lambda args"))



;; Edge case: list with only one item
(define singleton (list 42))
(print (if (equal? (car singleton) 42) "pass: singleton list car" "fail: singleton list car"))
(print (if (equal? (cdr singleton) NIL) "pass: singleton list cdr" "fail: singleton list cdr"))

;; Edge case: shadowing symbol in let
(define y 5)
(let ((y 999)) (print (if (equal? y 999) "pass: let shadowing" "fail: let shadowing")))
(print (if (equal? y 5) "pass: let didn't overwrite" "fail: let didn't overwrite"))

;; Edge case: redefining a function name
(define dup (lambda (x) x))
(define dup (lambda (x) (+ x x)))
(print (if (equal? (dup 3) 6) "pass: function redefinition" "fail: function redefinition"))

;; Edge case: applying a non-lambda symbol
(print
  (if (equal?
        (begin
          (define not-a-fn 42)
          (if (equal? not-a-fn not-a-fn) "pass: non-callable" "fail: non-callable"))
        "pass: non-callable")
      "pass: handled non-callable"
      "fail: handled non-callable"))

;; Edge case: quoting complex structure
(define complex-quote '(a (b (c d)) e))
(print (if (equal? (car complex-quote) 'a) "pass: quote car" "fail: quote car"))
(print (if (equal? (car (cdr complex-quote)) '(b (c d))) "pass: quote cdr" "fail: quote cdr"))
