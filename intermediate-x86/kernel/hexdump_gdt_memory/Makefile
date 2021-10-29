obj-m = hexdump_gdt_memory.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

install:
	rmmod hexdump_gdt_memory ; insmod hexdump_gdt_memory.ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean