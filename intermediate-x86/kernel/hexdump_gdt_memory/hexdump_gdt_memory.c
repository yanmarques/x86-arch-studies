#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>


static int __init my_module_init(void) {
    struct desc_ptr gdt;
    unsigned long long *ptr;
    unsigned long long *end;
    int i = 0;

    native_store_gdt(&gdt);
    printk(KERN_ALERT "GDT descriptor address: %#lx\n", gdt.address);
    printk(KERN_ALERT "GDT descriptor size: %#x\n", gdt.size);

    end = (long long *) gdt.address + gdt.size;

    for (ptr = (long long *) gdt.address; ptr < end; ptr++) {
        printk(KERN_ALERT "index: %d -> %#lx\n", i, *ptr);
        i++;
    }

    return 0;
}

static void __exit my_module_exit(void) {
    //
}

MODULE_LICENSE("GPL");
module_init(my_module_init);
module_exit(my_module_exit);
