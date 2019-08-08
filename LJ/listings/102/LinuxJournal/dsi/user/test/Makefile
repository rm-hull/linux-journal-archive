all:		fork dgram dgramresp servresp
fork:		fork.c
		gcc -Wall -o fork fork.c
dgram:          dgram.c
		gcc -Wall -o dgram dgram.c
dgramresp:	dgramresp.c
		gcc -Wall -o dgramresp dgramresp.c
servresp:	servresp.c
		gcc -Wall -o servresp servresp.c
clean:
		rm -f core fork dgram dgramresp servresp