;; a program execute the command-line as lisp

;; discard argv[0]
(setq args (cdr command-line-args-left))

;; if there are no arguments, read stdin and evaluate it
(or args
    (progn
      (find-file "/proc/self/fd/0")
      (eval-buffer)))

;; if there are arguments, evaluate them    
(while (car args)
  (eval (car (read-from-string (car args))))
  (setq args (cdr args)))
