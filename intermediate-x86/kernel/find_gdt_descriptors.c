#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>

static void print_descriptor(struct desc_struct *descriptor) {
    printk(KERN_ALERT "Limit: %#x %#x\n", descriptor->limit0, descriptor->limit1);
    printk(KERN_ALERT "Base: %#x %#x %#x\n", descriptor->base0, descriptor->base1, descriptor->base2);
    printk(KERN_ALERT "Descriptor Privilege Level: %d\n", descriptor->dpl);
    printk(KERN_ALERT "Type: %#x\n", descriptor->type);
    printk(KERN_ALERT "System: %#x\n", descriptor->s);
    printk(KERN_ALERT "Available (reserverd for developers): %#x\n", descriptor->avl);
    printk(KERN_ALERT "Long mode (x64): %#x\n", descriptor->l);
    printk(KERN_ALERT "D/B: %#x\n", descriptor->d);
    printk(KERN_ALERT "Granularity: %#x\n", descriptor->g);
    printk(KERN_ALERT "Present: %#x\n", descriptor->p);
}

static int __init my_module_init(void) {
    struct desc_ptr gdt;
    struct desc_struct *ptr;
    struct desc_struct *end;

    native_store_gdt(&gdt);
    printk(KERN_ALERT "GDT descriptor address: %#lx\n", gdt.address);
    printk(KERN_ALERT "GDT descriptor size: %#x\n", gdt.size);

    end = (struct desc_struct *) gdt.address + gdt.size;

    for (ptr = (struct desc_struct *) gdt.address; ptr < end; ptr++) {
        printk(KERN_ALERT "Descriptor address: %#lx\n", ptr);
        print_descriptor(ptr);
    }

    return 0;
}

static void __exit my_module_exit(void) {
    //
}

MODULE_LICENSE("GPL");
module_init(my_module_init);
module_exit(my_module_exit);