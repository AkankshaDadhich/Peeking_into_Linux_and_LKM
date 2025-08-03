KERNELRELEASE := $(shell uname -r)
KDIR = /lib/modules/$(shell uname -r)/build

# Common compiler flags
COMMON_CFLAGS := -std=gnu89 -g -Wall -Wno-unused-function -Wno-declaration-after-statement

# Kernel module settings
MODULE := ioctlChgParent
obj-m := $(MODULE).o

# User-space program settings
TARGETS := control_station soldier
SOURCES = $(TARGETS:=.c)

.PHONY: all clean

all: kernel_module user_space

kernel_module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user_space: $(TARGETS)

$(TARGETS): $(SOURCES)
	$(CC) $(COMMON_CFLAGS) -o $@ $@.c


clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c modules.order Module.symvers
	rm -rf .tmp_versions $(TARGETS)
