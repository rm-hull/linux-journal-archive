#
# Makefile for libddl.a (DJGPP GNU GCC compiler)
#
# By Carl J. Nobile
# Created: January 27, 1996
# Updated: 01/13/97
#
# MS-DOS makefile
#
CC	= gcc
RM	= rm
AR	= ar rcs

INCLUDE	= \djgpp\include
ERRFILE	= {$*}.err
DEBUG	= -g -DDEBUG
OPTIONS	= -O3 -m486 -ansi -fstrength-reduce -finline-functions -Wall
LIBS	= -L. -ldll -lm

#CFLAGS	= -DDOS -DGNU $(OPTIONS) 
CFLAGS	= $(OPTIONS) $(DEBUG)

#-------------------------------------------------

PROG	= dll_main
TEST	= dll_test
SRCS	= $(PROG).c $(TEST).c
OBJS1	= $(PROG).o
OBJS2	= $(TEST).o

#-------------------------------------------------

all	: libdll.a $(TEST).exe

.c.o	: $(SRCS)
	$(CC) $(CFLAGS) -c $< >$(ERRFILE)

libdll.a: $(OBJS1)
	$(AR) $@ $(OBJS1)

$(TEST).exe: $(OBJS2)
	$(CC) $(OBJS2) -o $(TEST) $(LIBS) >{linker}.err
	strip $(TEST)
	copy /b c:\djgpp\bin\stub.exe+$(TEST) $(TEST).exe
#	coff2exe -s c:/djgpp/bin/go32.exe $(TEST)
	$(RM) $(TEST)

$(PROG).o: $(PROG).c $(PROG).h
$(TEST).o: $(TEST).c linklist.h libdll.a
#-------------------------------------------------

clean	:
	$(RM) *.o *.err

clobber	:
	$(RM) *.o *.err *.exe libdll.a

rmlib	:
	$(RM) libdll.a

install	:
	copy libdll.a c:\djgpp\lib\libdll.a
	copy linklist.h c:\djgpp\include\linklist.h
