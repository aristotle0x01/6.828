

## Lab2

## **File descriptors**

- xv6 kernel uses the file descriptor as an index into a per-process table, so that every process has a private space of file descriptors starting at zero
- Two file descriptors **share an offset** if they were derived from the same original file descriptor by a sequence of fork and dup calls
- A newly allocated file descriptor is always the lowest-numbered unused descriptor of the current process

### **APIs**

- fork

  fork copies the parent’s file descriptor table along with its memory

  underlying file offset is shared between parent and child

  ```
  if(fork() == 0) { 
     write(1, "hello ", 6); exit();
  } else {
     wait();
     write(1, "world\n", 6);
  }
  
  // hello world
  ```

- exec

  replaces the calling process’s memory but preserves its file table

- dup

  both file descriptors share an offset

  ```
  fd = dup(1);
  write(1, "hello ", 6); 
  write(fd, "world\n", 6);
  
  // hello world
  ```

### **Pipes**

```
  int p[2];
  char *argv[2];
  argv[0] = "wc"; 
  argv[1] = 0;

  pipe(p); 
  if(fork() == 0) {
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);
    exec("/bin/wc", argv);
  } else {
    close(p[0]);
    write(p[1], "hello world\n", 12);
    close(p[1]);
  }
```

```
echo hello world | wc
echo hello world >/tmp/xyz; wc </tmp/xyz
```



## process abstraction mechanism

- user/kernel mode flag

  kernel / user stack

  hardware privilege level

- address spaces

  page table

  kernel / user space arrangement?

  MMU

- time-slicing

  context switch



## Questions

### lab2

- physical mem management
- virtual/phy mem mapping
- kernel/user mem privilege

### general

- what's segmentation?

- what's paging and why is it needed?

- privilege level implementation with segmentation and paging?

### specifics

- The indirect jump is needed because the assembler would otherwise generate a PC-relative direct jump, which would execute the low-memory version of main?
- Modify xv6 so that the pages for the kernel are shared among processes, which reduces memory consumption

### ref

**IA-32 **Volume 3(System Programming Guide)**: **chapter 3 protected-mode memory management

[Intel 80386 Reference Programmer's Manual](https://pdos.csail.mit.edu/6.828/2018/readings/i386/toc.htm) : Chapter 5-Memory Management & Chapter 6-Protection

[x86_translation_and_registers](https://pdos.csail.mit.edu/6.828/2018/lec/x86_translation_and_registers.pdf)

------

### general: segmentation

segmentation is a way of dividing memory for programs: 

- flat model
- protected flat model
- multi-segmented model

<img src="./raw/lab2-1.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

most os today uses flat model, which amounts to not using segmentation. Since it would require programs all loaded in memory, does not make economical sense.

#### logical, effective, linear and physical address

<img src="./raw/lab2-2.jpg?raw=true" alt="ssh_port" style="zoom:40%;float: left" />

<img src="./raw/lab2-3.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

#### **Segment Selectors**

<img src="./raw/lab2-4.jpg?raw=true" alt="ssh_port" style="zoom:50%;" />

- index: 15-3, support 8192 descriptors in GDT
- TI: 0=GDT, 1=LDT
- RPL: **Requested Privilege Level (RPL)**, RPL & CPL & DPL

##### **segment Registers**

<img src="./raw/lab2-5.jpg?raw=true" alt="ssh_port" style="zoom:50%;" />

> When a segment selector is loaded into the visible part of a segment register, the processor also loads the hidden part of the segment register with the base address, segment limit, and access control information from the segment descriptor pointed to by the segment selector

two kinds of instructions for loading segment registers:

1. MOV, POP, LDS, LES, LSS, LGS, and LFS
2. CALL, JMP, and RET, IRET, INT*n*, INTO and INT3, these instructions change the CS register

##### **segment & system Descriptors**

if the **<u>S</u>** bit 0 for system descriptor, 1 for segment descriptor

<img src="./raw/lab2-6.jpg?raw=true" alt="ssh_port" style="zoom:50%;" />

**segment descriptor**:GDTR Register

> A segment descriptor is a data structure in a GDT or LDT that provides the processor with the size and location of a segment, as well as access control and status information. Segment descriptors are typically created by compilers, linkers, loaders, or the operating system or executive, but not application programs

two types of segment descriptor (stack is also data): code and data

<img src="./raw/lab2-7.jpg?raw=true" alt="ssh_port" style="zoom:40%;float: left" />

> All data segments are nonconforming, meaning that they cannot be accessed by less privileged programs or procedures
>
> (code segment) A transfer of execution into a more-privileged conforming segment allows execution to continue at the current privilege level

*Global and Local Descriptor Tables*

<img src="./raw/lab2-9.jpg?raw=true" alt="ssh_port" style="zoom:30%;float: left" />

**system descriptor**: IDTR register

- Local descriptor-table (LDT) segment descriptor
- Task-state segment (TSS) descriptor
- Call-gate descriptor
- Interrupt-gate descriptor
- Trap-gate descriptor
- Task-gate descriptor

<img src="./raw/lab2-8.jpg?raw=true" alt="ssh_port" style="zoom:40%;float: left" />



看一看代码层面的数据结构；看一下执行时的情况

分析一下GDT加载的情况

### general: paging

#### why paging?

To effiently share physical memory between multiple tasks, given them a 4Gb consistent view of address space.

#### basics

- **PG (paging) flag.** Bit 31 of CR0
- **PDBR**: the physical address of the current page directory is stored in the CR3 register
- **TLBs**: the processor stores the most recently used page-directory and page-table entries in on-chip caches called translation lookaside buffers or TLBs.  **INVLPG** instruction is provided to invalidate a specific page-table entry in the TLB

#### translation

<img src="./raw/lab2-11.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

------

<img src="./raw/lab2-10.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />



### lab2 paging

#### configuration

**enable paging**

```
# Load the physical address of entry_pgdir into cr3.  entry_pgdir
# is defined in entrypgdir.c.
movl	$(RELOC(entry_pgdir)), %eax
movl	%eax, %cr3
# Turn on paging.
movl	%cr0, %eax
orl	$(CR0_PE|CR0_PG|CR0_WP), %eax
movl	%eax, %cr0
```

**initial paging directory**

```
pde_t entry_pgdir[1024] = {
	// Map VA's [0, 4MB) to PA's [0, 4MB)
	[0] = ((uintptr_t)entry_pgtable - KERNBASE) + PTE_P,
	// Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
	[KERNBASE>>PDXSHIFT] = ((uintptr_t)entry_pgtable - KERNBASE) + PTE_P + PTE_W
};

// Entry 0 of the page table maps to physical page 0, entry 1 to
// physical page 1, etc.
pte_t entry_pgtable[1024] = {
	0x000000 | PTE_P | PTE_W,
	0x001000 | PTE_P | PTE_W,
	...
	0x3ff000 | PTE_P | PTE_W }
```

#### jos paging implementation

<img src="./raw/lab2-14.png?raw=true" alt="ssh_port" style="zoom:100%;float: left" />





### privilege level checking with segmentation and paging

#### protection rings

<img src="./raw/lab2-15.png?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

#### RPL & CPL & DPL

- **Descriptor privilege level (DPL).** The DPL is the privilege level of a segment or gate. It is stored in the DPL field of the segment or gate descriptor for the segment or gate
- **Current privilege level (CPL)**: The CPL is the privilege level of the currently executing program or task. It is stored in bits 0 and 1 of the CS and SS segment registers. Normally, the CPL is equal to the privilege level of the code segment from which instructions are being fetched
- **Requested privilege level (RPL).** The RPL is an override privilege level that is assigned to segment selectors. It is stored in bits 0 and 1 of the segment selector.
  - A **segment selector** is a 16 bit value held in a segment register

**important facts**

- before the processor loads a segment selector into a segment register, it performs a privilege check

  <img src="./raw/lab2-16.png?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

- implicit use of segment registers, I.e. “ESP” == “SS:ESP”, “EIP” == “CS:EIP”

##### **how check is done: data segment**

$$
max(CPL, RPL) ≤ DPL
$$

<img src="./raw/lab2-17.png?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

<img src="./raw/lab2-18.png?raw=true" alt="ssh_port" style="zoom:45%;float: left" />

from above pics, only after the check passed can the segment descriptor content be put into correponding register designated by the segment selector, and with base, limits, access info put into hidden parts. That is to say segment selector is just a dynamic temp value, is under software control.



##### **how check is done**: **stack segment**

CPL == RPL == DPL



##### **how check is done**: **code segment**

1. no privilege-level checks for near forms of the JMP, CALL, and RET instructions transfer program control within the current code segment

2. for far form accessing nonconforming code segments, **CPL == DPL and RPL <= CPL**

   <img src="./raw/lab2-19.png?raw=true" alt="ssh_port" style="zoom:40%;" />

3. for far form accessing conforming code segments, **CPL >= DPL, RPL won't be checked**

   > When program control is transferred to a conforming code segment, the CPL does not change, even if the DPL of the destination code segment is less than the CPL. This situation is the only one where the CPL may be different from the DPL of the current code segment. Also, since the CPL does not change, no stack switch occurs.
   >
   > Conforming segments are used for code modules such as math libraries and exception handlers, which support applications but do not require access to protected system facilities

4. **accessing a Code Segment Through a Call Gate**

   <img src="./raw/lab2-20.png?raw=true" alt="ssh_port" style="zoom:40%;" />

   <img src="./raw/lab2-21.png?raw=true" alt="ssh_port" style="zoom:40%;" />

   <img src="./raw/lab2-22.png?raw=true" alt="ssh_port" style="zoom:40%;" />

5. stack switching

   > Whenever a call gate is used to transfer program control to a more privileged nonconforming code segment (that is, when the DPL of the nonconforming destination code segment is less than the CPL), the processor automatically switches to the stack for the destination code segment’s privilege level. This stack switching is carried out to prevent more privileged procedures from crashing due to insufficient stack space. It also prevents less privileged procedures from interfering (by accident or intent) with more privileged procedures through a shared stack
   >
   > 
   >
   > Pointers to the privilege level 0, 1, and 2 stacks are stored in the TSS for the currently running task (see Figure 6-2). Each of these pointers consists of a segment selector and a stack pointer (loaded into the ESP register)

   <img src="./raw/lab2-23.png?raw=true" alt="ssh_port" style="zoom:40%;" />

   

6. **returning from a Called Procedure**

   > The RET instruction can be used to perform a near return, a far return at the same privilege level, and a far return to a different privilege level. This instruction is intended to execute returns from procedures that were called with a CALL instruction. It does not support returns from a JMP instruction, because the JMP instruction does not save a return instruction pointer on the stack

7. **privileged instructions**

   They can be executed only when the CPL is 0:

   - LGDT—Load GDT register.
   - LLDT—Load LDT register.
   - LTR—Load task register.
   - LIDT—Load IDT register.
   - MOV (control registers)—Load and store control registers.
   - INVLPG—Invalidate TLB entry.
   - HLT—Halt processor.
   - ...

##### page protection

> Page-level protection can be used alone or applied to segments. When page-level protection is used with the flat memory model, it allows supervisor code and data (the operating system or executive) to be protected from user code and data (application programs). It also allows pages containing code to be write protected

1. protection information for pages is contained in two flags in a page-directory or page-table entry: the read/write flag (bit 1) and the user/supervisor flag (bit 2)

2. supervisor mode vs user mode

   > If the processor is currently operating at a CPL of 0, 1, or 2, it is in supervisor mode, it can access all pages; if it is operating at a CPL of 3, it is in user mode, it can access only user-level pages

3. page type

   - Read-only access (R/W flag is 0)
   - Read/write access (R/W flag is 1)

4. two level page-directory and the page-table

   <img src="./raw/lab2-24.png?raw=true" alt="ssh_port" style="zoom:30%;" />

5. overrides to Page Protection

   access to segment descriptors in the GDT, LDT, or IDT

##### **COMBINING PAGE AND SEGMENT PROTECTION**

> When paging is enabled, the processor evaluates segment protection first, then evaluates page protection. If the processor detects a protection violation at either the segment level or the page level, the memory access is not carried out and an exception is generated. If an exception is generated by segmentation, no paging exception is generated.
>
> Page-level protections cannot be used to override segment-level protection
>
> Page-level protection can be used to enhance segment-level protection



##### **ref**

IA-32-3: 4.6 PRIVILEGE LEVEL CHECKING WHEN ACCESSING DATA SEGMENTS





## skill sets

**physical & virtual page table views**:

- qemu

  ```
  info mem
  info pg
  ```

- **xp paddr** vs **x vaddr**

  view content in virtual & physical address, should be the same 

**knowledge**:

- BIOS
- ELF
- c calling convention: [BUFFER OVERFLOW 6 The Function Stack](https://www.tenouk.com/Bufferoverflowc/Bufferoverflow2a.html)
- i/o port development: [Chapter 6: Parallel I/O ports](https://users.ece.utexas.edu/~valvano/Volume1/E-Book/C6_MicrocontrollerPorts.htm)
- makefile
- C: K & R

