;; a program to compile an elisp file.
;; I would like to use stdin and stdout by it's past the deadline,
;; and I have to time to refresh my E-Lisp knowledge

;; the current file name is still in the cmd line, so remove it
;; the second item is the source,
;; the third item, if any is the target

(setq myname (car command-line-args-left))
(setq args (cdr command-line-args-left))
(setq source (car args))
(setq args (cdr args))
(setq target (car args))

(or source (message "Usage: %s <source> [<target>]" myname))
(and source (byte-compile-file source))

(and target
     (progn (rename-file (concat source "c") target t)
	    (message "Renamed output file to \"%s\"" target)))

(or target (setq target (concat source "c")))

;; finally, give execute permission to the target
(call-process "chmod" nil nil nil "+x" target)

