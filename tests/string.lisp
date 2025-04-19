(equal? "hello" "hello")     ;; => #t
(equal? "hello" "world")     ;; => ()
(define greet "hi")
greet                        ;; => "hi"
(print "Done!")              ;; (if you have `print`)