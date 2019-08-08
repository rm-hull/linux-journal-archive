#
# Makefile for LLIBDLL.lib  (MS C Version 6.0)
#
# By Carl J. Nobile
# Created: January 10, 1997
# Updated: 01/13/97
#
CC	= cl
LD	= link
RM	= rm
AR	= lib

LIB	= .\;c:\c600\lib
ERRFILE	= {$*}.err
DEBUG	= -DDEBUG -Od -Zi
OPTIONS	= -Ox -W4 -AL
CFLAGS	= $(OPTIONS) $(DEBUG) /F A000

#-------------------------------------------------
PROG	= dll_main
TEST	= dll_test
SRCS	= $(PROG).c $(TEST).c
OBJS1	= $(PROG).obj
OBJS2	= $(TEST).obj

#-------------------------------------------------
.SUFFIXES : .obj

all	: llibdll.lib $(TEST).exe

.c.obj	: $(SRCS)
	$(CC) $(CFLAGS) -c $< >$(ERRFILE)

llibdll.lib: $(OBJS1)
	$(RM) $@
	$(AR) $@ +$(OBJS1),

$(TEST).exe: $(OBJS2)
	$(CC) $(OBJS2) $(CFLAGS) llibdll.lib >{link}.err

$(PROG).obj: $(PROG).c $(PROG).h
$(TEST).obj: $(TEST).c linklist.h llibdll.lib

#-------------------------------------------------
clean	:
	$(RM) *.obj *.err

clobber	:
	$(RM) *.obj *.err *.exe llibdll.lib

rmlib	:
	$(RM) llibdll.lib

install	:
	copy llibdll.lib c:\c600\lib\llibdll.lib
	copy linklist.h c:\c600\include\linklist.h
