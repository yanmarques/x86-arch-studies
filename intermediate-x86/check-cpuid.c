
int main() {
    __asm(
        "xor %rax, %rax\n"
        "cpuid\n"
    );
}