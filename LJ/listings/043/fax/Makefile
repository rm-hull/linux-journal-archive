
CC=gcc
LIBS=-Lcgic105/ -lcgic
INCL=-Icgic105/
ADDFLAGS = -O -Wall

CFLAGS = ${LIBS} ${INCL} ${ADDFLAGS}

all: cgic fax.cgi status.cgi install

cgic : 
	make -C cgic105

fax.cgi : fax.c cgic105/libcgic.a
	${CC} ${CFLAGS} -o fax.cgi fax.c

status.cgi : status.c cgic105/libcgic.a
	${CC} ${CFLAGS} -o status.cgi status.c

install :
	cp fax.cgi /home/httpd/html/fax/
	cp status.cgi /home/httpd/html/fax
