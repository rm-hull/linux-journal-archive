#Makefile
DIR_PREFIX=/usr1/mico

all: .depend consumer supplier 

include ./MakeVars

CXXFLAGS  := $(COS_CXXFLAGS) $(CXXFLAGS)
LDLIBS    := $(COS_LDLIBS) $(LDLIBS) -lpthread
LDFLAGS   := $(COS_LDFLAGS) $(LDFLAGS)
DEPS      := $(COS_DEPS) $(DEPS)

INSTALL_DIR     = events
INSTALL_SRCS    = Makefile supplier.C consumer.C
INSTALL_SCRIPTS = runit

consumer: $(OBJS) consumer.o $(DEPS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) consumer.o $(LDLIBS) -o $@

supplier: $(OBJS) supplier.o $(DEPS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) supplier.o $(LDLIBS) -o $@

clean:
	rm -f .depend *.o core consumer supplier *~ *# 0*
