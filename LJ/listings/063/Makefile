# Makefile

.SUFFIXES: .java .class .idl .module

.java.class:
	vbjc $<

.idl.module:
	idl2java $<
	touch $@

default: all

clean: 
	rm -rf logging
	rm -f *.class *.tmp *.module *~

IDLS = \
	EventChannel.idl

MODULES = $(IDLS:.idl=.module) 

SRCS = \
	PullConsumer.java \
	PushSupplier.java \
	LJEventChannel.java 

CLASSES = $(SRCS:.java=.class) 

all:	$(MODULES) $(CLASSES)




