
ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= read.o write.o

else

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif


clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -rf .tmp_versions

