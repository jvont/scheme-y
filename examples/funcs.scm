
(define (println x) (display x) (newline))

(define (add1 x) (+ x 1))
(define (sub1 x) (- x 1))

(define pi (* 4 (atan 1.0)))
(define deg2rad
  (lambda (deg) (/ (* deg pi) 180)))

(newline)
(println (add1 2))
(println (sub1 3))
(println (deg2rad 180))
