;; Basic true branch
(print (if (equal? 
  (cond ((equal? 1 1) "yes") 
        (#t "no"))
  "yes") "pass: cond true" "fail: cond true"))

;; False then true
(print (if (equal? 
  (cond ((equal? 1 2) "nope") 
        ((equal? 2 2) "match") 
        (#t "fail"))
  "match") "pass: cond second branch" "fail: cond second branch"))

;; Fallback to #t
(print (if (equal? 
  (cond ((equal? 1 2) "bad") 
        (#t "fallback"))
  "fallback") "pass: cond fallback" "fail: cond fallback"))

;; Multiple falses before #t
(print (if (equal? 
  (cond ((equal? 1 2) "wrong")
        ((equal? 2 3) "nope")
        ((equal? 4 5) "nah")
        (#t "default"))
  "default") "pass: cond many falses" "fail: cond many falses"))

;; Nested cond with #t as final fallback
(print (if (equal? 
  (cond ((equal? 0 1) "wrong")
        ((equal? 1 1) (cond ((equal? 2 2) "nested") (#t "fallback")))
        (#t "outer fallback"))
  "nested") "pass: cond nested" "fail: cond nested"))

;; cond with numbers
(print (if (equal? 
  (cond ((equal? 1 2) 100)
        ((equal? 3 3) 42)
        (#t 0))
  42) "pass: cond numeric" "fail: cond numeric"))
