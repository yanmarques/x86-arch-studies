#include <stdio.h>

void highload() {
    __asm(
        "mov $0x1, %rax\n"
        "cpuid\n"
    );
}

int main() {
    __asm(
        // SAVE START CYCLES
        "rdtsc\n"
        "push %rax\n"

        // EXECUTE THE DESIRED CODE TO PROFILE
        "call highload\n"        

        // FETCH END CYCLES AND CALCULATE HOW MANY CYCLES IT TOOK
        "rdtsc\n"
        "xor %rcx, %rcx\n"
        "pop %rcx\n"
        "sub %eax, %ecx\n"

        // PREPARE ARGUMENTS TO PRINTF@PUTS
        // 1. put message in memory: '[+] took %08x cycles'
        "mov $0x0a73656c637963, %rax\n"
        "push %rax\n"
        "mov $0x2078383025783020, %rax\n"
        "push %rax\n"
        "mov $0x6b6f6f74205d2b5b, %rax\n"
        "push %rax\n"

        // set message pointer
        "mov %rsp, %rdi\n"

        // 2. integer is the subtraction done earlier
        "mov %ebx, %esi\n"
        "mov $0x0, %eax\n"

        "call printf\n"

        // CLEANUP THE STACK
        "pop %rax\n"
        "pop %rax\n"
        "pop %rax\n"
    );
}