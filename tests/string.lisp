(print "hello world")         ;; → hello world
(print (+ 5 6))               ;; → 11
(print (cons 1 2))            ;; → (1 . 2)
(print (lambda (x) x))        ;; → (lambda (x) x)
(print (equal? "hello" "hello"))    ;; => #t
(print (equal? "hello" "world"))    ;; => ()
(print (define greet "hi"))
(print greet)                       ;; => "hi"
(print "Done!")              ;; (if you have `print`)