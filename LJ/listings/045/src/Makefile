CFLAGS = -O -Wall
all: bluff.o bloom.o fail.o

clean:
	rm -f bluff.o bloom.o fail.o *~

%: %.el
	./lisp-compiler $< $@


