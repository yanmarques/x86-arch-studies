#include <stdlib.h>

// sys_open : 2
// const char *filename : %rdi
// int flags : %rsi
// int mode : %rdx

// sys_read: 0
// unsigned int fd : %rdi
// const char *buf : %rsi
// size_t count : %rdx

int main() {
    __asm(
        // OPEN

        "xor %rsi, %rsi\n"
        "xor %rdx, %rdx\n"

        "push %rsi\n"                       // put null byte to terminate string

        // put "/etc/passwd" into the stack
        "mov $0x6477737361702f63, %rbx\n"    // c/passwd
        "push %rbx\n"

        "mov $0x74652f2f, %ebx\n"            // te//
        "sub $0x4, %rsp\n"
        "mov %ebx, (%rsp)\n"

        "mov %rsp, %rdi\n"                  // filename ptr

        "push $0x2\n"                       // sys_open
        "pop %rax\n"
        "syscall\n"

        // READ

        "mov %rax, %rdi\n"                  // int fd

        "sub $0x500, %rsp\n"                // allocate 1280 bytes in memory

        "mov %rsp, %rsi\n"                  // char* buffer

        "mov $0x500, %rdx\n"                // size_t count

        "xor %rax, %rax\n"                  // sys_read
        "syscall\n"      
    );
}