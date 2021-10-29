obj-m = mycpl.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

install:
	rmmod mycpl ; insmod mycpl.ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean