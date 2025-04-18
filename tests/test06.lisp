(define outer 100)
(define shadow
  (lambda (outer) (lambda () outer)))

((shadow 42))