;; This test file deliberately triggers evaluator errors,
;; and expects them to return error objects (not crash).

;; Attempt to call a non-function
(print (print 42))               ; OK
(print (42 1 2))                 ; ERROR: 42 is not callable

;; Wrong number of args to lambda
(define id (lambda (x) x))
(print (id))                     ; ERROR: missing arg
(print (id 1 2))                 ; ERROR: too many args

;; Type mismatch
(print (+ 1 'x))                 ; ERROR: 'x is not a number
(print (* "hello" 5))           ; ERROR: "hello" is not a number

;; Accessing car of an atom
(print (car 42))                ; ERROR: not a pair
(print (cdr "not a list"))      ; ERROR: not a pair

;; Bad cond form
(print (cond (42)))             ; ERROR: malformed cond clause
(print (cond ((equal? 1 1))))   ; ERROR: missing result in clause

;; Bad define
(print (define 42 99))          ; ERROR: define must use symbol
(print (define (42 x) x))       ; ERROR: function name must be a symbol

;; Unbound symbol
(print unknown-variable)        ; ERROR: unbound symbol
