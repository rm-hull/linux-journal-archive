SHELL		= /bin/sh

#****************************************************************************
# CHANGE THE NEXT LINE SO THAT IT POINTS TO THE BASE DIRECTORY OF YOUR
# OMNIORB INSTALLATION (I have omniORB installed in /usr1/omniORB_2.5.0):
top_srcdir	= /usr1/omniORB_2.5.0
#****************************************************************************

SERVER_NAME	= server
CLIENT_NAME	= client
SKEL_NAME	= PushStringSK.cc
SKEL_HDR		= PushString.hh

TARGETS		= $(SERVER_NAME) $(CLIENT_NAME) $(SKEL_NAME) $(SKEL_HDR)

include Make.rules

COMMON_OBJS	= PushStringSK.o
SERVER_OBJS	= PushString_i.o Srv_Main.o
CLIENT_OBJS	= Client.o
SRCS	= $(COMMON_OBJS:.o=.cc) \
		  $(SERVER_OBJS:.o=.C) \
		  $(CLIENT_OBJS:.o=.C)

ALL_CXXFLAGS	= $(CXXFLAGS) 
ALL_CPPFLAGS	= $(CPPFLAGS) -I. -I$(top_srcdir)/include -I$(top_srcdir)
ALL_LDFLAGS	= $(LDFLAGS)
ALL_DEFS	= $(DEFS)
ALL_LIBS	= $(OMNI_LIBS) $(LIBS)

PushStringSK.cc PushString.hh: PushString.idl
	if test -x $(IDL) ; then \
	rm -f PushStringSK.cc PushString.hh ; \
	$(IDL) $(IDL_FLAGS) PushString.idl ; \
	fi

$(SERVER_NAME): $(COMMON_OBJS) $(SERVER_OBJS)
	rm -f $@
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) -o $@ \
	$(COMMON_OBJS) $(SERVER_OBJS) $(ALL_LIBS)

$(CLIENT_NAME): $(COMMON_OBJS) $(CLIENT_OBJS)
	rm -f $@
	$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) -o $@ \
	$(COMMON_OBJS) $(CLIENT_OBJS) $(ALL_LIBS)

clean::
	rm -f $(TARGETS)

