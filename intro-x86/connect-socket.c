// sys_socket : 41
// int family : %rdi
// int type : %rsi
// int protocol : %rdx

// sys_connect : 42
// int fd : %rdi
// struct sockaddr_in *uservaddr : %rsi
// int addrlen : %rdx

// sys_close : 3
// unsigned int fd : %rdi

int main() {
    __asm(
        // CREATE SOCKET
        "mov $0x2, %rdi\n"              // family AF_INET
        "mov $0x1, %rsi\n"              // type SOCK_STREAM
        "xor %rdx, %rdx\n"              // protocol IP

        "mov $0x29, %rax\n"             // sys_socket
        "syscall\n"

        "mov %rax, %rbx\n"               // save fd

        // CONNECT
        "push $0x0100007f\n"             // sockaddr_in.sin_addr 127.0.0.1
        "pushw $0x2923\n"                // sockaddr_in.sin_port 9001
        "pushw $0x2\n"                   // sockaddr_in.sin_family AF_INET

        "mov %rbx, %rdi\n"              // fd
        "mov %rsp, %rsi\n"              // *uservadrr
        "mov $0x10, %rdx\n"             // addrlen 16 bytes of sockaddr_in

        "mov $0x2a, %rax\n"             // sys_connect
        "syscall\n"

        "sub $0x10, %rsp\n"             // caller cleanup. struct uservadrr is 16 bytes long

        // CLOSE
        "mov %rbx, %rdi\n"              // fd
        "mov $0x3, %rax\n"              // sys_close
        "syscall\n"
    );
}