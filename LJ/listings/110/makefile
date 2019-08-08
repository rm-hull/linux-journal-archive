#the compiler
CPP = g++
CC = gcc

#compiler options

CPPOPTS = -O3 -g -Wall -c $(shell sdl-config --cflags) -Wno-deprecated -ansi

#header files
INCLUDES = \
	-I. \
	$(NULL)

#library directories
LIBDIRS = \
	$(NULL)

#libraries to link with
LIBS = \
	-lSDL_ttf \
	$(NULL)

#all compiler flags
CFLAGS = $(CPPOPTS) $(INCLUDES)

#all linker options and libs
LFLAGS = $(LIBDIRS) $(shell sdl-config --libs)

#default compilation rules
%.o:	%.cpp
	$(CPP) -c $< $(CFLAGS) -o $@

%.o:	%.c
	$(CC) -c $< $(CFLAGS) -o $@

#local headers to depend on
HEADERS = \
	$(NULL)

#files to compile
CCFILES = \
	$(NULL)

CPPFILES = \
	bounce.cpp	\
	$(NULL)

#all local .o files
OBJS =  ${CPPFILES:.cpp=.o} ${CCFILES:.c=.o}

default: depend bounce

bounce: $(OBJS) $(HEADERS) 
	$(CPP) $(LFLAGS) $(OBJS) $(LIBS) -o $@

depend: ${CPPFILES} ${CCFILES} ${HEADERS} makefile
	rm -f depend ; ${CPP} -MM -c ${CFLAGS} ${CPPFILES} ${CCFILES} > depend

clean:
	rm -f core $(OBJS) bounce depend *~

#
# get the depend info
#
include depend
