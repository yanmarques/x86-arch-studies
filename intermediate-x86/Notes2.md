## Topic Day 2 Part 3

- 32 to 32 4MB.
- why would one want ginormous pages? The operating system stores common used code inside huge pages to reduce TLB misses (cache)

### TLB (Translation Lookaside Buffer)

- an in-cpu cache which stores linear addresses to physical pages
- performance reasons
- instruction `INVLPG` invalidate the TLB cache entry for a specified virtual memory address
- TLB caches:
    - Data TLBs: one for 4KB and other for (2MB/4MB) translations
    - Instruction TLBs: one for 4KB and other for (2MB/4MB) code access

### Shadow Walker Rootkit

1. Patch Page Fault Handler (PFH) with a custom code
2. The custom code, hides read/write data access to the Rootkit virtual memory. How? Check the requested Page Number is the virtual memory where the Rootkit is in, and fill the Data TLB with some random junk memory address
3. In order for the Rootkit code to continue running, when the requested access is code, it fills the Instruction TLB with the correct virtual address

Defeating Shadow Walker:
- check Instruction Descriptor  Table (IDT) page fault handler entry to ensure it is the default handler the OS uses
- integrity check the memory for the legitimate page fault handler to ensure it isn't subject to inline hook
- manually flush TLB entries
- map each physical page of memory to a target virtual memory and look that target virtual memory


### Physical Address Extension (PAE)

- hardware-supported way to address more than 4GB of RAM
- Windows uses different kernel when DEP (Data Execution Prevention), ntkrnlpa.exe instead of regular ntoskrnl.exe

- Entry Formats: Pretty much like regular PDE and PTE, but the last bit is the XD (Execution Disabled), when set to 1 the mapped address may not be executable (used by DEP)
- CR3 register now points to an exact PDE address, it is not 4KB aligned as normally

- By convention, the Page Directories (physical memory pointed by CR3) are mapped to 0xC060000. To find the PDE of a given VA (Virtual Address), you compute:
    `0xC0600000 + (upper 11 bits of VA) * sizeof(PDE)`, where `sizeof(PDE) == 8`

- By convention, the Page Tables are mapped starting at 0xC0000000. The PTE of a given VA (Virtual Address):
    `0xC0000000 + (upper 11 bits of VA) * PAGE_SIZE + (middle 9 bits of VA) * sizeof(PTE)`, where `PAGE_SIZE == 4096` and `sizeof(PTE) == 8`

### NX/XD bit

- the bit implements `W^X` (write XOR execute), so a memory can be write or executable, not both.
- PTE, PDE, PDPTE may set the bit (higgher granularity especifications of NX override finer granularity, off course)
- Microsoft refers NX to DEP (Data Execution Prevention), or "Hardware DEP". "Software DEP" refers to SafeSEH

### Inter-Process Communication (IPC)

- Processes can map chunks of physical memory into their virtual memory space to share that chunks of memory. Eg.: VirtualAlloc()/VirtualProtect() on Windows; mmap()/mprotect() on *nix

### Day 2 Part 4

#### Instruction: RDTSC

- Read Time-Stamp Counter. The result is a 64 bit value, stored into EDX:EAX (upper 32 in edx and lowers in eax)
- Set to 0 when processor reset, incremented on each clock cycle

- RDTSC in practice:
    - timing code for performance reasons, cound how many seconds did the code take to run
    - check presence of debuggers by timing own execution

#### Interrupts And Exceptions

- Events that indicate that somewhere in the system requires the processor attentions
- Two sources of interrupts:
    - Hardware
    - Software

- Difference between interrupts and exceptions: exceptions typically indicate an error, and the interrupt clears the Interrupt Flag (IF)
- 3 categories of exceptions:
    - Fault: can be recovered. Eg.: Illegal Instruction when EIP points at faulting instruction, the processor can use a default address to continue the execution and report the error
    - Trap: can be recovered: EIP points to the instruction following the trap instruction
    - Abort: can not be recovered

#### Saving State

- When a current procedure needs to be suspended, what happens?
- The OS nees to save some state of the current procedure in order to know where to execute after suspension. The OS saves the state in some stack:
    - When the next procedure is in the same ring of the current procedure, it saves on the stack of the `current procedure`. The data saved is: `EFLAGS, CS, EIP and Error Code`
    - Otherwise, when the next procedure is more privileged, it saves in the stack of the `next procedure`. The data saved this time is: `SS, ESP, EFLAGS, CS, EIP and Error Code`

- How the OS knows the address of the stack of the next procedure to save data? It consults the structure called Task State Segment (TSS)

#### Tasks

- How to access the Task State Segment (TSS)? Through the 16-bit register called Task Register (TR), which points to an entry into GDT of Task Segment type. (Load/store task register with `LTR` and `STR` respectively)
- The task segment points to the Task State Segment, just a large data structure of mostly saved register values

- So, when an interrupt occurs and the handler (or next procedure) is going to take control, the OS:
    1. takes the value of TR register and find the entry in GDT with that value as index (shift left value by 3)
    2. get the base address of the task segment and find the structure in TSS
    3. the address of the stack of the next procedure is there (`SS0` if ring 0, `SS1` if ring 1, `SS2` if ring 2), along a bunch of data that I actually do not know

### Interrupt Descriptor Table (IDT)

- An array <= 256 64bit descriptor entries. Index 1 to 31 are reserved to arch-specific and the rest is user defined.
- One can think of it like an array of function pointers, where the *nth*descriptor is the function of the interrupt `n`

- No.: 1, Mnemonic: #DB, Description: Debugger (Hardware Breakpoints), Type: Fault/Trap, Error Code: no
- No.: 3, Mnemonic: #BP, Description: Breakpoint (Software Breakpoints), Type: Trap, Error Code: no
- No.: 13, Mnemonic: #GP, Description: General Protection, Type: Fault, Error Code: yes

#### The SegFault lie

- The common `Segmentation Fault` or `SIGSEGV` (signal segment violation) is like a catch all error generated on *nix systems when a process tries to access memory that is not supposed to. But actually the hardware will report General Protection or Page Fault errors, via interrupts.

### Day 2 part 5

#### Software generated Interrupts

- Instruction: `int n` - Go to IDT[n] and execute the function. **Important: some interrupts expects an error code, so if you call the interrupt instruction with some interrupt of type error make sure you push an erro code**
- Instruction: `iret` - Return from interrupt, popping all saved state back
- Instruction: `int 3` - Same as opcode `0xCC`
- Instruction: `into` - invoke the `Overflow` interrupt, same as `int 4` if Overflow Flag (OF) is set to 1
- Instruction: `ud2` - invoke invalid opcode interrupt (`int 6`)

#### IDTR

- How to find Interrupt Descriptor Table? By the IDTR register.
- `lidt` to set the address of IDT (privileged instruction), `sidt` store the address into memory (non-privileged instruction)
- The address in IDTR register is 6 bytes long, where the upper 32 bits is the logical base address of the table, and the lower 16 bits are the table size

- Interrupt descriptors: 8 byte structures of 3 categories:
    - Trap Gate
    - Task Gate
    - Interrupt Gate
- The Trap and Interrupts descriptors are similar, the only difference is that in the interrupt, the IF and EFLAGS are cleared
- The upper 32 bits of the descriptor are part of the logical address, where the upper 16 bits are the segment selector and the lower 16 bits are the offset. So the hardware set the CS to the segment selector and jump to the offset.
- So, how the OS control ring 3 to ring 0 interrupts? By the DPL field inside the descriptor. When the interrupt occurs, only DPLs <= descriptor's DPL can actually continue with the interrupt. Eg.: if a gate has DPL 0 and a userspace of DPL 3 interrupts to that gate, access is denied. 

#### Interrupt Masking

- Sometimes the kernel code does not want to listen for interrupts for a moment, for example when handling an interrupt, then it may set the IF flag to 0, so the hardware ignores any incoming interrupts (with some exceptions, off course). 
- One can clear the IF flag with `cli` instruction, of manually set the flag with `sli {0|1}`. Note that this is a ring0 only instruction, because the userspace is not responsible enough to decide to stop receiving interrupts, like, the mouse would stop working, screen, networking, and so on.
- So, the kernel generally uses the Interrupt Gate just because it automatically clears the IF flag, and can handle each interrupt without _interrupts_.
- The userspace _could_ set IF flag but changing the saved EFLAGS in the stack when switching contexts, but this is not achievable for sure.

### Day 2 Part 6

#### Rootkit hook detector evasion

- Normal kernel hooks changes the 32 bit offset to attackers code. But one can obtain the attacker code without changing the 32 bit offset, but creating an entry into the GDT with a base address which:
    - `ATTACKERS_ADDR = BASE_ADDR + HOOK_DEFAULT_ADR`
    
Then just change the segment selector of the hook to the one created. That way, the 32 bit offset doesn't change and may evade some rootkit detectors. 

Another way is to use a negative offset with a negative base address. (still unclear why that may be good)

#### Interrupts and Debuggers

- The debugger overwrite the code with the opcode `0xCC` to add a breakpoint. When the kernel execute the software breakpoint, the debugger insert the original opcode back and the continue executing.

- Hardware breakpoints can be set to trigger when memory is read or written.
- There are 8 debug registers (DR-0-DR7)
- Accessing those registers requires CPL == 0
    - DR0-3: set the address you wanna break
    - DR4-5: reserved
    - DR6: tell what breakpoint just happened
    - DR7: control how to trigger breakpoint. Eg.: turn on write, read, or execute

- Local Enable just enabled breakpoints for the created task. Global enable the breakpoint no matter the task.
- Local and global exact breakpoint: if enabled, enables the detection of the exact instruction which caused a data breakpoint

- General Detect (GD): if set to 1, handle when someone tries to change the debug registers. It does work as a security mechanism, because the attacker can hijack the hook handler.
- DR7: Specify what breakpoint access to listen to. 
RW0-3 is interpreted as follows:
    - `0x00`: execution only
    - `0x01`: data writes only
    - if (CR4.DE == 1) then `0x10`: I/O reads and writes
    - if (CR4.DE == 0) then `0x10`: Undefined
    - `0x11`: data reads and writes but no instruction fetches

LEN0-4: specify what size the address stored in the DR0-3 registers should be treated as:
    - `0x00`: 1 byte
    - `0x01`: 2 bytes
    - `0x10`: undefined (or 8 bytes)
    - `0x11`: 4 bytes

- Resume Flag (RF): if enabled, processor ignores instruction breakpoints for the duration of the next instruction. The processor automatically clears this flag after the instruction returned to has been successfully executed
- Trap Flag (TF): if enabled, it causes a debug exception interrupt after every instruction. This is called "single-step" mode.

#### Malware: Anti-debugging

- Pack the code with a key, store the key into Debug Registers, then try to unpack the code from the Debug Registers data. It only will unpack if no hardware debugging was set.

- Userspace can actually modify Debug Registers values on Windows. Because it saves thread information between context switches, and it has an api to allow userspace to get and set this thread information, so malware can screw up hardware debugging information.

#### Branch Trace Store

- In order to debug a malware execution one could detect what paths the code executes. It could be done with single-step interrupts, set a hardware breakpoint at the beggining and enable single-mode with the Trap Flag (TF), register eip, continue; but this way it costs too much from the hardware. A new way to do that is with branch tracing, where the hardware loggs every branch instructions like `call`s and `jmp`s into a Branch Trace Store (BTS) buffer memory
