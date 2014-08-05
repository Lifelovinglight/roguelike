(use-modules (srfi srfi-1)
	     (srfi srfi-31)  ; Anonymous recursion
	     (srfi srfi-41)  ; Streams
	     (srfi srfi-42)) ; Comprehensions

(define (inc n)
  (+ n 1))

(define (dec n)
  (- n 1))

(define (x point)
  (car point))

(define (y point)
  (cadr point))

(define (z point)
  (caddr point))

(define (north point)
  (list (x point)
	(dec (y point))
	(z point)))

(define (east point)
  (list (inc (x point))
	(y point)
	(z point)))

(define (south point)
  (list (x point)
	(inc (y point))
	(z point)))

(define (west point)
  (list (dec (x point))
	(y point)
	(z point)))

(define (down point)
  (list (x point)
	(y point)
	(dec (z point))))

(define (up point)
  (list (x point)
	(y point)
	(inc (z point))))

(define key-c 99)

(define key-d 100)

(define key-w 119)

(define key-s 115)

(define key-r 114)

(define key-stairs-up 60)

(define key-stairs-down 62)

(define key-plus 43)

(define key-up (key_up))

(define key-down (key_down))

(define key-left (key_left))

(define key-right (key_right))

(define air 0)

(define (air? c)
  (= c air))

(define wall 1)

(define (wall? c)
  (= c air))

(define slope 2)

(define (slope? c)
  (= c slope))

(define door-closed 3)

(define (door-closed? c)
  (= c door-closed))

(define door-open 4)

(define (door-open? c)
  (= c door-open))

(define ladder 5)

(define (ladder? c)
  (= c ladder))

(define (passable? c)
  (or (air? c)
      (door-open? c)
      (ladder? c)))

(define nothing 0)

(define soil 1)

(define grass 2)

(define rock 3)

(define wood 4)

(define concrete 5)

(define steel 6)

(define (open-door point)
  (if (door-closed? (terrain point))
      (set-terrain point door-open)))

(define (close-door point)
  (if (door-open? (terrain point))
      (set-terrain point door-closed)))

(define (above? x y)
  (> (z x)
     (z y)))

(define (player-move-horiz start destination)
  (let ((ts (terrain start))
	(td (terrain destination)))
    (cond ((and (slope? (terrain (down start)))
		(passable? (terrain (down destination))))
	   (down destination))
	  ((passable? td)
	   destination)
	   ((and (slope? td)
		 (passable? (terrain (up destination))))
	    (up destination))
	   ((door-closed? (terrain destination))
	    (begin (open-door destination)
		   start))
	  (else start))))

(define (player-move-vert start destination)
  (let ((ts (terrain start))
	(td (terrain destination)))
    (if (passable? td)
	(cond ((above? start destination)
	       destination)
	      ((ladder? ts)
	       destination)
	      (else start))
	start)))

(define (move-north point)
  (player-move-horiz point (north point)))

(define (move-east point)
  (player-move-horiz point (east point)))

(define (move-south point)
  (player-move-horiz point (south point)))

(define (move-west point)
  (player-move-horiz point (west point)))

(define (move-down point)
  (player-move-vert point (down point)))

(define (move-up point)
  (player-move-vert point (up point)))

(define (build point c m)
  (let ((direction (input-direction point)))
  (begin (set-terrain direction c)
	 (set-material direction m)
	 point)))

(define (dig point)
  (build point air nothing))

(define (build-wall point)
  (build point wall concrete))

(define (build-slope point)
  (build point slope concrete))

(define (build-door point)
  (build point door-closed steel))

(define (build-ladder point)
  (build point ladder steel))

(define (my-assoc x l)
  (cond ((null? l) #f)
	((eq? x (caar l))
	 (cadar l))
	(else (my-assoc x (cdr l)))))

(define direction-translation
  (list (list key-up north)
	(list key-down south)
	(list key-left west)
	(list key-right east)
	(list key-stairs-down down)
	(list key-stairs-up up)))

(define input-translation
	  (list (list key-up move-north)
		(list key-right move-east)
		(list key-down move-south)
		(list key-left move-west)
		(list key-stairs-down move-down)
		(list key-stairs-up move-up)
		(list key-d dig)
		(list key-w build-wall)
		(list key-s build-slope)
		(list key-plus build-door)
		(list key-c (lambda (point)
			      (begin (close-door (input-direction point))
				     point)))
		(list key-r build-ladder)))

(define (input-direction point)
  (let ((input (my-assoc (getch) direction-translation)))
    (if input
	(input point)
	(input-direction point))))

(define (fall point)
  (if (not (or (ladder? (terrain point))
	       (ladder? (terrain (down point)))))
      (if (passable? (terrain (down point)))
	  (fall (down point))
	  point)
      point))

(define (player point input)
  (fall (let ((action (my-assoc input input-translation)))
	  (if action
	      (action point)
	      point))))

(define (random-cardinal-direction)
  (my-assoc (random 4)
	    (list (list 0 north)
		  (list 1 east)
		  (list 2 south)
		  (list 3 west))))

(define (random-cardinal-neighbour point)
  (let ((neighbour ((random-cardinal-direction) point)))
    (if (not (out-of-bounds neighbour))
	neighbour
	(random-cardinal-neighbour point))))

(define (transform-terrain point from to)
  (if (= (terrain point) from)
      (set-terrain point to)))

(define (random-walk terrain depth point)
  (if (zero? depth)
      point
      (begin (transform-terrain point wall terrain)
	     (random-walk terrain
			  (dec depth)
			  (random-cardinal-neighbour point)))))

(define (ladder-shaft-up height point)
  (if (zero? height)
      point
      (begin (set-terrain point ladder)
	     (set-material point steel)
	     (ladder-shaft-up (dec height) (up point)))))

(define (ladder-shaft-to-surface point)
  (if (> (z point) 9)
      point
      (begin (set-terrain point ladder)
	     (set-material point steel)
	     (ladder-shaft-to-surface (up point)))))

(define (generate-cave-level point)
  (ladder-shaft-up 2 (random-walk air 100 point)))

(define (generate-cave-levels point height)
  (if (zero? height)
      point
      (generate-cave-levels
       (generate-cave-level point)
       (dec height))))

(define (generate-terrain)
  (set! *random-state* (random-state-from-platform))
  (ladder-shaft-to-surface (generate-cave-levels '(40 12 0) 4)))
