;; 1. Global cache
(define fib-cache ())

;; 2. Assoc: find (key . value) pair
(define assoc
  (lambda (key alist)
    (if (equal? alist ())
        ()
        (if (equal? (car (car alist)) key)
            (car alist)
            (assoc key (cdr alist))))))

;; 3. Lookup: extract value from assoc pair
(define lookup
  (lambda (key alist)
    (cdr (assoc key alist))))

;; 4. Get from cache
(define fib-get
  (lambda (n)
    (if (equal? (assoc n fib-cache) ())
        ()
        (lookup n fib-cache))))

;; 5. Put in cache using set!
(define fib-put!
  (lambda (n result)
    ((lambda (ignored)
       result)
     (set! fib-cache (cons (cons n result) fib-cache)))))

;; 6. Memoized Fibonacci
(define fib
  (lambda (n)
    (if (equal? (fib-get n) ())
        (fib-put! n
          (if (equal? n 0)
              0
              (if (equal? n 1)
                  1
                  (+ (fib (- n 1)) (fib (- n 2))))))
        (fib-get n))))

(fib 5000)