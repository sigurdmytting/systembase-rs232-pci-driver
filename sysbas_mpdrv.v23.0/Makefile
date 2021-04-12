# Copyright 2009-2012 SYSTEMBASE all right reserved 
# for kernel 2.6 & 3.0
#
DRIVERNAME := golden_tulip
all:	modules 

install: modules
	mkdir -p /lib/modules/$(shell uname -r)/kernel/drivers/char/
	cp -f ./$(DRIVERNAME).ko /lib/modules/$(shell uname -r)/kernel/drivers/char/
	mkdir -p /lib/modules/$(shell uname -r)/misc/
	cp -f ./$(DRIVERNAME).ko /lib/modules/$(shell uname -r)/misc/
	depmod -a

	
	
ifneq ($(KERNELRELEASE),)

obj-m += $(DRIVERNAME).o
$(DRIVERNAME)-y := sb_pci_mp.o
else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)


modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean: clean_local


endif


clean_local:
	rm -f *~
	rm -f *.o
	rm -rf ./.tmp_versions
	rm -f *.mod*
	rm -f *.ko
	rm -f .*.cmd
	rm -f $(DRIVERNAME)
	rm -f Module*
	rm -f module*
#	rm -f /lib/modules/$(shell uname -r)/kernel/drivers/char/$(DRIVERNAME).ko
#	rm -f /lib/modules/$(shell uname -r)/misc/$(DRIVERNAME).ko
	rm -f sb_config

