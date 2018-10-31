
ifneq ($(KERNELRELEASE),)
obj-m := chardev.o
else
KDIR := /lib/modules/4.9.0-2-rt-amd64/build
all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
endif
