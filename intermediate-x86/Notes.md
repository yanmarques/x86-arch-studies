## Day 1 Part 2

### CPUID - CPU (feature) Identification

- receive input at eax and output in eax, ebx, ecx and edx.
- CPUID not added until late intel 486 models
- how to check whether the instruction is available? Try to set the ID flag, if it changes then the instruction is supported.
- stored at bit 21 of EFLAGS

### PUSHFD - Push EFLAGS onto stack

- PUSHF pushes the 16 lower bits of EFLAGS

### POPFD - Pop stack into EFLAGS

- popf pops the 16 lower bits into EFLAGS
- some flags are not allowed to be changed unless in ring 0

### Segmentation

- provides a mechanism for dividing processor's addressable memory space into smaller protected address spaces called segments
- a segment descriptor has a base address, an access mode and the address limit

#### Addressing

- to find a byte inside a segmentation, one must use logical address. First get the segment descriptor and find the base address, then calculate the offset of the memory from that base address.

- so, segmentation translates logical addresses to linear addresses (physical memory addresses) automatically in hardware by using lookup tables

#### Selectors

- a 16 bit value held in a segment register. It is used to find a segment descriptor from one of the two tables:

    - GDT: Global Descriptor Table - for use system-wide
    - LDT: Local Descriptor Table - per-process and switched when the kernel switches between process contexts

- the table index is actually 13 bits not 16, so it can hold 2^13 = 8192 descriptors. The other 3 bits are used as following:

    - lowest 2 bits are the RPL (Request Privilege Level)
    - the 3º lowest bit is the table indicator, 0 for GDT and 1 to LDT

#### The six segment registers

- CS: Code segment - hardware implicitly uses this segment to find the next instruction in EIP
- SS: Stack segment - data segments which must be read/write, otherwise a general-protection exception is generated
- DS: data segment - same as SS
- ES/FS/GS: extra (usually data)

- the hardware fetches a descriptor once, then cache the whole descriptor inside a hidden space not available through software

#### Implicit use of segment registers

- `pop %ebp` == `pop ss:%ebp`
- `jmp $0x40123` == `jmp cs:$0x40123`


#### Extra

- hardware checks when access is out of the segment bound
- hardware also validates whether access to a given segment is allowed from the RPL field


#### Global Descriptor Table Register (GDTR)

- the upper 32 bits are the physical address
- the lower 16 bits specify the size of the table
- `LGDT`: load 6 bytes from memory into GDT (privileged instruction, only ring 0 can access)
- `SGDT`: store 6 bytes from GDT into memory

#### Local Descriptor Table Register (LDTR)

- `LLDT`: load 16 bits segment descriptor from memory into LDT (privileged instruction, only ring 0 can access)
- `SLDT`: store 6 bytes from LDT into memory

#### Descriptor description

- Base (32 bits): physical memory address where the segment starts
- Limit (20 bits): size of segment. End address = base + limit
- G(ranularity): if 0, interpret Limit as size in bytes. Otherwise, interpret as size in 4kb blocks
- D/B: default operation size, 0 for 16 bit and 1 for 32 bit
- DPL (Descriptor Privilege Level) - range from 0 to 3, 0 being the most privileged
- L flag: 64 bit segment - ignore
- S (System): 0 for system segment, 1 for code or data segment
- Type (4 bits): whether a segment is code or data, permissions and other stuff
- P(resent) flag: whether the segment exists

### Extra

- if ring 3 tries to jump/call/ret to a Code Segment at ring 0, hardware denies access. Access to any other ring more privileged then the current ring results in access denied.

- the `mov` instruction can not be used to load the CS register. Imagine why is that?

# Day 1 part 4

## Call Gates

- a way to transfer control from one segment to another, even with different ring levels

- to use a call gate one must specify the segment index of the call gate and the offset of the function to call:
```c
__asm("call 0x28:0x123456")
```

# Day 1 Part 5

## Paging

- paging is the method of finding a word in memory, which one must first lookup the Page Directory, then the Page Table and finally finding the physical memory
- before turning on paging, the OS had a 1:1 map of linear address to physical address
- the OS puts addresses in the linear address of the segment like a little data structure

### Translate linear to physical memory

- traditional:
    - 32:32 bits, 4kb pages
    - 32:32 bits, 4MB pages (called huge pages)
- physical address extension (PAE):
    - 32:36, 4kb pages
    - 32:36, 2MB pages

### The Control Registers

- 5 Control Registers (CR0-CR4) which are used for paging management, like enabling, configuring
- CR0 has the PG bit which may be set to 1 to enable paging
- CR0 also has the PE bit which may be set to 1 to enable protected mode (PG requires PE being set)
- CR1 is reserved
- CR2 stores the linear address that caused a page fault exception
- CR3 store the base physical address of the Page Directory structure, AKA Page Directory Base Register (PDBR). It is a per-process thing, so the OS can separate the memory view of each process correctly
- CR4 stores bits to configure:
    - PAE
    - PSE - Page Size Extensions which enables/disables the use of large (4MB or 2MB) pages
    - PGE - Page Global Enable, what is global page feature?? Global pages are maintained in cache between process contexts switches


### Accessing Control Registers

- they are accessed in their own `mov` instructions which only allows register to register data transfers

### Memory sharing

- every process has it's own Page Directory, but the Page Tables could be shared, like shared C libraries

### Accessing Virtual Memory

- CR3 register points to a physical address. The upper 20 bits are considered the address and the remaining 12 bits are assumed to be 0 because of aligment of 4kb pages
- from the linear address, where:
    - the 10 upper bits are Page Directory's index
    - the 10 middle bits are Page Table's index
    - the remaining 12 bits are page's offset
one can find the PDE (Page Directory Entry) with = CR3 + (8 * value of 10 upper bits of linear address)
- the 20 upper bits of the PDE points to the Page Table base address that contains the PTE (Page Table Entry)
- the middle 10 bits of the linear address are used as index to the Page Table base address in order to find the Page
- the PTE points to the base address of that Page physical memory, and the 12 lower bits of the linear address are used as offset to access the desired memory

- when a process closes the kernel is responsible for freeing the pages of that dead process. But, in order to not leak information to another process, the pages being reallocated needs to pass through a sanitizing process (AKA zeroing out)


### Page Directory Entry (PDE) fields

- P(resent) flag: whether the pointed page table is present in physical memory
- R/W (Read/Write) flag: if 1, rw otherwise is read-only. If the PTE says different, the restricter mode is considered
- U/S (User/Supervisor): if 1, pages are accessed by CPL == 3 (user mode), if 0, pages can only be accessed by CPL <= 3 (supervisor mode)
- U/S and R/W. There is a flag in CR0 register, WP (Write Protect), that when set to 1, even supervisor mode can't write to a read-only page. It's not a security feature, but an error prevention one.
- A (accessed) flag: whether the page was read or written already.
- PS (page size): 0 for 4kb pages, 1 for huge pages (4MB or 2MB)
- G (global): disables keeping paging lookups caches at the Translation Lookupaside Buffer (TLB)
- Page-Table Base Address: upper 20 bits of the **physical address** for the base of a Page Table.

### Page Table Entry (PTE) fields

- P, R/W, U/S and G flags are exact the same as with PDE
- Page Base Address: upper 20 bits of the **physical address** for the base of a Page.

### Important

There is not fixed way that virtual memory are mapped to physical memory. Different OSes applies different strategies.

### Windows PDE/PTE organization

- By convention, the Page Directory is mapped to virtual address `0xC0300000`. To find the PDE for a given virtual address, you compute:

```
0xC0300000 + (upper 10 bits of VA) * sizeof(PDE)
* sizeof(PDE) = 8 if x64 bit else 4
```

- By convention, the Page Table is mapped into virtual memory starting at `0xC0000000`. To find the PTE for a given virtual address, you compute:

```
0xC0000000 + (upper 10 bits of VA) * PAGE_SIZE + (middle 10 bits of VA) * sizeof(PTE)

* PAGE_SIZE = 4kb = 4096
* sizeof(PTE) = 8 if x64 bit else 4 
```

- Example:

