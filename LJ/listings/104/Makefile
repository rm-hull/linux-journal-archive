# Makefile for compiling the files for ptrace article

# A few variables

CC=gcc
LIBS=-lncurses

SRC_DIR=.
EXE_DIR=.

EXES = \
    ${EXE_DIR}/attach \
	${EXE_DIR}/changesyscall\
	${EXE_DIR}/breakpoint\
	${EXE_DIR}/freespaceinject\
	${EXE_DIR}/inject\
	${EXE_DIR}/registers\
	${EXE_DIR}/simple\
	${EXE_DIR}/singlestep\
	${EXE_DIR}/syscallparams\
	${EXE_DIR}/dummy1\
	${EXE_DIR}/dummy2\

${EXE_DIR}/%: %.o
	${CC} -o $@ $<

%.o: ${SRC_DIR}/%.c
	${CC} -o $@ -c $<

%.o: ${SRC_DIR}/%.s
	${CC} -o $@ -c $<


all:    ${EXES}

clean:
	@rm -f ${EXES}
