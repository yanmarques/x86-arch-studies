#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

struct selector {
    unsigned int rpl: 2;
    unsigned int table: 1;
    unsigned short offset: 13;
} __attribute__((packed));

static int __init my_module_init(void) {
    unsigned short csSelector = 0;
    struct selector *cs;

    asm volatile("mov %%cs, %0": "=m"(csSelector));
    cs = (struct selector *)(&csSelector);

    printk(KERN_ALERT "cs selector: %#x\n", csSelector);
    printk(KERN_ALERT "rpl: %d\n", cs->rpl);
    printk(KERN_ALERT "table: %s\n", cs->table == 1 ? "LDT" : "GDT");
    printk(KERN_ALERT "offset: %#x\n", cs->offset);

    return 0;
}

static void __exit my_module_exit(void) {
    //
}

MODULE_LICENSE("GPL");
module_init(my_module_init);
module_exit(my_module_exit);