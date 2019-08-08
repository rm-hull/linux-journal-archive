CC=gcc

LINUX=/usr/src/linux
MODCFLAGS	:= -g -Wall -DMODULE -D__KERNEL__ -DLINUX -O2 -I$(LINUX)/include -fomit-frame-pointer -fno-strict-aliasing -march=i686 -mpreferred-stack-boundary=2

#MODCFLAGS	:= -D__KERNEL__ -I$(LINUX)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe -mpreferred-stack-boundary=2 -march=i686 -DMODULE -DMODVERSIONS -include $(LINUX)/include/linux/modversions.h

all:		lsm.o

lsm.o:		startup.o dsi.o dsi_task.o dsi_cache.o dsi_inode.o dsi_net.o 
		ld -m elf_i386 -r -o lsm.o startup.o dsi.o dsi_task.o dsi_cache.o dsi_inode.o dsi_net.o

startup.o:	startup.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c startup.c
dsi.o:		dsi.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c dsi.c
dsi_task.o:     dsi_task.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c dsi_task.c
dsi_cache.o:	dsi_cache.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c dsi_cache.c
dsi_inode.o:    dsi_inode.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c dsi_inode.c
dsi_net.o:	dsi_net.c /usr/src/linux/include/linux/version.h dsi.h
		$(CC) $(MODCFLAGS) -c dsi_net.c

clean:
		rm -f *.o core






