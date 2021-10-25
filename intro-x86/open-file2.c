#include <stdlib.h>

// sys_open : 2
// const char *filename : %rdi
// int flags : %rsi
// int mode : %rdx

// sys_read: 0
// unsigned int fd : %rdi
// const char *buf : %rsi
// size_t count : %rdx

// sys_fstat: 5
// unsigned int fd : %rdi
// struct stat *statbuf : %rdi


int main() {
    __asm(
        // OPEN

        "xor %rsi, %rsi\n"                      // null byte and param flags (RDONLY)
        "xor %rdx, %rdx\n"                      // null byte and param mode (ignored)

        // put "/etc/passwd" into the stack
        "push $0x64777373\n"                    // "dwss"

        "mov $0x61702f6374652f2f, %rbx\n"       // "//etc/pa"
        "push %rbx\n"

        "mov %rsp, %rdi\n"                      // filename

        "mov $0x2, %rax\n"                      // sys_open
        "syscall\n"
        "pop %rdi\n"                            // caller cleanup
        "pop %rdi\n"

        "mov %rax, %rdi\n"                      // save fd

        // STAT

        "sub $0x90, %rsp\n"                     // allocate sizeof(struct stat) = 144 bytes
        "mov %rsp, %rsi\n"                      // *statbuf

        "mov $0x5, %rax\n"                      // sys_fstat
        "syscall\n"
        
        "lea 0x30(%rsi), %rbx\n"                // st_size is the sixth parameter, then 6*8 = 0x30

        // READ

        "sub (%rbx), %rsp\n"                    // allocate space in memory for file
        "mov %rsp, %rsi\n"                      // *buffer

        "mov (%rbx), %rdx\n"                    // count

        "xor %rax, %rax\n"                      // sys_read
        "syscall\n"
    );
}