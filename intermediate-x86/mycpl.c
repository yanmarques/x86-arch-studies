#include <stdio.h>

struct selector {
    unsigned int rpl: 2;
    unsigned int table: 1;
    unsigned short offset: 13;
} __attribute__((packed));

int main() {
    unsigned short csSelector = 0;
    struct selector *cs;

    asm volatile("mov %%cs, %0": "=m"(csSelector));
    cs = (struct selector *)(&csSelector);

    printf("cs selector: %#lx\n", csSelector);
    printf("rpl: %d\n", cs->rpl);
    printf("table: %s\n", cs->table == 1 ? "LDT" : "GDT");
    printf("offset: %#lx\n", cs->offset);
}