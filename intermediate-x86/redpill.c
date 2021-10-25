#include <stdio.h>

int main() {
    __asm(
        // Allocate 10 bytes to store IDTR
        "sub $0xa, %rsp\n"

        // Store IDTR address into stack
        "sidt (%rsp)\n"

        // Load 9th byte into %rbx 
        "lea 0x9(%rsp), %rcx\n"
        "mov (%rcx), %rbx\n"

        // mask 6 upper bits
        "and $0x000000ff, %rbx\n"
        
        // Cleanup the stack
        "add $0xa, %rsp\n"

        // Prepare to call printf
        // 1. set message
        "mov $0x0a78383025, %rax\n"
        "push %rax\n"
        "mov $0x7830206574796220, %rax\n"
        "push %rax\n"
        "mov $0x7473616c205d2b5b, %rax\n"
        "push %rax\n"
        
        // Set message pointer argument
        "mov %rsp, %rdi\n"

        // Set IDT last byte argument
        "mov %rbx, %rsi\n"
        "xor %rax, %rax\n"

        // Print to screen
        "call printf\n"

        // Cleanup stack
        "pop %rax\n"
        "pop %rax\n"
        "pop %rax\n"
    );
}