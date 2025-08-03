KDIR =  /lib/modules/$(shell uname -r)/build  
obj-m += get_pgfaults.o hello_procfs.o
kbuild:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

