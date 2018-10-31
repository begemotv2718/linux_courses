
ifneq ($(KERNELRELEASE),)
obj-m := mini2c.o
else
KDIR := ${HOME}/rpi-course/s2/sample-packages/buildroot/output/build/linux-custom/
all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
endif
