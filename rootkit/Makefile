ifneq ($(KERNELRELEASE),)

# Build one module from multiple source files
obj-m := syscall.o

else

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif