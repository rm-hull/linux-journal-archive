CC=gcc
LINUX=/usr/src/linux
MODCFLAGS	:= -Wall -I$(LINUX)/include -I../lsm
#GTKFLAGS	:= `pkg-config --cflags --libs gtk+-2.0` -DGTK_GUI
GTKFLAGS	:=

all:		SetNodeID UpdatePolicy CheckAlarm

SetNodeID:	SetNodeID.c
		$(CC) $(MODCFLAGS) -o SetNodeID SetNodeID.c
UpdatePolicy:	UpdatePolicy.c
		$(CC) $(MODCFLAGS) -o UpdatePolicy UpdatePolicy.c
CheckAlarm:	CheckAlarm.c
		$(CC) $(MODCFLAGS) $(GTKFLAGS) -o CheckAlarm CheckAlarm.c

clean:
		rm -f *.o SetNodeID UpdatePolicy CheckAlarm core
