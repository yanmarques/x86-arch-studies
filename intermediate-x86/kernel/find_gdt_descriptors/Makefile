obj-m = find_gdt_descriptors.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

install:
	rmmod find_gdt_descriptors ; insmod find_gdt_descriptors.ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean