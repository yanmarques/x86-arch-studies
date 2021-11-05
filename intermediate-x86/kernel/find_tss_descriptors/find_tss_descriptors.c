#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc_defs.h>

#define TSS_BUSY "TSS (busy)"
#define TSS_AVL "TSS (available)"

static int __init my_module_init(void) {
    unsigned short trSelector;
    unsigned long *tssBaseAddress;
    unsigned int tssLimit;
    struct desc_ptr gdt;
    struct ldttss_desc *tssGdt;

    asm volatile("str %0": "=m"(trSelector));
    printk(KERN_ALERT "TSS Selector: %#x", trSelector);

    asm volatile("sgdt %0": "=m"(gdt));
    printk(KERN_ALERT "GDT address: %#lx", gdt.address);

    tssGdt = (struct ldttss_desc *)(gdt.address + trSelector);

    printk(KERN_ALERT "TSS gdt descriptor: %#lx", tssGdt);
    printk(KERN_ALERT "TSS gdt descriptor -> type: %s (%d)", tssGdt->type == 11 ? TSS_BUSY : TSS_AVL, tssGdt->type);

    tssBaseAddress = tssGdt->base0 | (tssGdt->base1 << 16) | (tssGdt->base2 << 24) | ((unsigned long)tssGdt->base3) << 32;
    tssLimit = tssGdt->limit0 | (tssGdt->limit1 << 16);

    printk(KERN_ALERT "TSS base address: %#lx", tssBaseAddress);
    printk(KERN_ALERT "TSS limit: %#x", tssLimit);

    printk(KERN_ALERT "TSS struct: %#lx", *tssBaseAddress);

    return 0;
}

static void __exit my_module_exit(void) {
    //
}

MODULE_LICENSE("GPL");
module_init(my_module_init);
module_exit(my_module_exit);
