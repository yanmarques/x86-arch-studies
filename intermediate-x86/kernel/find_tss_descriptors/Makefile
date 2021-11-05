obj-m = find_tss_descriptors.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

install:
	rmmod find_tss_descriptors ; insmod find_tss_descriptors.ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean