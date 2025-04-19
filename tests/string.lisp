(print "hello world")         ;; → hello world
(print (+ 5 6))               ;; → 11
(print (cons 1 2))            ;; → (1 . 2)
(print (lambda (x) x))        ;; → (lambda (x) x)
(equal? "hello" "hello")     ;; => #t
(equal? "hello" "world")     ;; => ()
(define greet "hi")
greet                        ;; => "hi"
(print "Done!")              ;; (if you have `print`)