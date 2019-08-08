
SRCS=gctest.c gctest_parseopts.c gctest_aux.c gctest_instrument.c
LIBS=-lgc -ldl -lm

gctest: $(SRCS) gctest.h
	gcc $(SRCS) -o gctest $(LIBS)

