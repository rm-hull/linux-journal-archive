# Comment/uncomment the following line to enable/disable debugging
DEBUG = y

# Change it here or specify it on the "make" commandline
INCLUDEDIR = /usr/include

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSAD_DEBUG
else
  DEBFLAGS = -O2
endif

CFLAGS = -D__KERNEL__ -DMODULE -Wall $(DEBFLAGS)

CFLAGS += -I$(INCLUDEDIR)

OBJS = sad.o

all: $(OBJS)

install:
	mkdir -p /lib/modules/$(VER)/misc /lib/modules/misc
	install -c $(OBJS) /lib/modules/$(VER)/misc
	install -c $(OBJS) /lib/modules/misc

clean:
	rm -f *.o *~ core
