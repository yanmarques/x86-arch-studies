cmd_/home/user/Documents/intermediate-x86/kernel/find_gdt_descriptors.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 --build-id  -T ./scripts/module-common.lds -o /home/user/Documents/intermediate-x86/kernel/find_gdt_descriptors.ko /home/user/Documents/intermediate-x86/kernel/find_gdt_descriptors.o /home/user/Documents/intermediate-x86/kernel/find_gdt_descriptors.mod.o;  true
