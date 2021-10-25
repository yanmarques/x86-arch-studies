/**
 * Find the GDT and LDT tables
 */

#include <stdio.h>

struct desc_ptr {
    unsigned short size;
    unsigned long address;
} __attribute__((packed));

int main() {
    struct desc_ptr gdt, ldt;

    asm volatile("sgdt %0": "=m" (gdt));
    printf("GDT register: %#lx%x\n", gdt.address, gdt.size);
    printf("GDT table limit: %#x (%d)\n", gdt.size, gdt.size);
    printf("GDT base address: %#lx\n", gdt.address);

    printf("\n\n");

    asm volatile("sldt %0": "=m" (ldt));
    printf("LDT register: %#lx%x\n", ldt.address, ldt.size);
    printf("LDT table limit: %#x (%d)\n", ldt.size, ldt.size);
    printf("LDT base address: %#lx\n", ldt.address);
}