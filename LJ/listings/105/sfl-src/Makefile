# Makefile for sendfile example program
# Dragan Stancevic <visitor@xalien.org>
#

CFLAGS=-Wall -O2 -s
CC=gcc

all: clean sfl data

data:
	dd if=/dev/urandom of=data.bin bs=1k count=10

clean:
	rm -f sfl data.bin
