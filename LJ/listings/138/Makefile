CFLAGS= -g
OBJS= hextract.o printoctal.o hex2octal.o hex2dec.o main.o arrayp.o decrypt.o
deshc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
clean: 
	rm -f ${OBJS}
