

## dev tips

- make run-**faultread**-nox

  and the like without changing code in "ENV_CREATE(user_hello, ENV_TYPE_USER);"

- make grade

  not work locally, but you can inspect grade-lab3 by comparing outputs 



## answers-lab3

1. What is the purpose of having an individual handler function for each exception/interrupt? (i.e., if all exceptions/interrupts were delivered to the same handler, what feature that exists in the current implementation could not be provided?)

   Ans:  trapno wouldn't  be provided, thus you don't know what kind of interrupt acctually happened.

2. Did you have to do anything to make the `user/softint` program behave correctly? The grade script expects it to produce a general protection fault (trap 13), but `softint`'s code says `int $14`. *Why* should this produce interrupt vector 13? What happens if the kernel actually allows `softint`'s `int $14` instruction to invoke the kernel's page fault handler (which is interrupt vector 14)?

   Ans:  because its interrupt descriptor privilege level is ring 0, cann't be called from ring 3/user level



## **exercises**

### **exercise 9**

If you now run `user/breakpoint`, you should be able to run backtrace from the kernel monitor and see the backtrace traverse into `lib/libmain.c` before the kernel panics with a page fault. What causes this page fault? 

<img src="./raw/lab3-1.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

<img src="./raw/lab3-2.jpg?raw=true" alt="call convention" style="zoom:35%;float: left" />

<img src="./raw/lab3-3.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

Referenced user empty page, so page faulted



## Lab3

### key points

***On the whole, user process, memory(paging) and interrupt implementation in this lab; should have a clear picture of user/kernel isolation and interaction***

- struct Env

  ```
  struct Env {
  	struct Trapframe env_tf;	// Saved registers
  	struct Env *env_link;		// Next free Env
  	envid_t env_id;			// Unique environment identifier
  	envid_t env_parent_id;		// env_id of this env's parent
  	enum EnvType env_type;		// Indicates special system environments
  	unsigned env_status;		// Status of the environment
  	uint32_t env_runs;		// Number of times environment has run
  
  	// Address space
  	pde_t *env_pgdir;		// Kernel virtual address of page dir
  };
  ```

  - env_init()
  -         env_setup_vm()
  -         region_alloc()
  -         env_create()
  -         env_run()
  -         load_icode()
  -         env_pop_tf()

- handling interrupts and exceptions

  - interrupt descriptor table
  - the task state segment
  - stack change & trapframe
  - priviledge level check

  `trapentry.S trap.c`

  <img src="./raw/lab3-10.jpg?raw=true" alt="call convention" style="zoom:25%;float: left" />

- page fault & why it shouldn't happen in kernel 

- breakpoint & debug & FL_TF flag enabling single-step trace

- system call 

  pass system call number in AX, up to five parameters in DX, CX, BX, DI, SI. Interrupt kernel with T_SYSCALL

- user-mode startup

- memory protection: check user-supplied pointer with **`user_mem_check`**



### user & kernel spaces

<img src="./raw/lab3-kernel-space.png?raw=true" alt="call convention" style="zoom:50%;float: left" />

### interrupt priviledge

#### **interrupt descriptor table (IDT)**

<img src="./raw/lab3-4.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

<img src="./raw/lab3-5.jpg?raw=true" alt="call convention" style="zoom:35%;float: left" />

#### **Interrupt-Handler Procedures**

<img src="./raw/lab3-6.jpg?raw=true" alt="call convention" style="zoom:35%;float: left" />



##### privilege-level check

- interrupt vectors have no RPL, the RPL is not checked
- if interrupt is generated with INT *n*, INT 3, or INTO. then **CPL <= DPL of the interrupt gate** 
- hardware interrupts and processor-detected exceptions, processor **ignores** the DPL of interrupt gates



##### stack change

- If handler procedure is to be executed at a numerically lower privilege level, **a stack switch occurs**
- If handler procedure is to be executed at the same privilege level, no stack switch
- the processor uses the **`ESP0` and `SS0`** fields of the **TSS** to define the kernel stack when entering kernel mode. JOS doesn't use any other TSS fields

```
// Task state segment format
struct Taskstate {
	uintptr_t ts_esp0;	// Stack pointers and segment selectors
	uint16_t ts_ss0;	//   after an increase in privilege level
	...
	uintptr_t ts_eip;	
	uint32_t ts_eflags;
	uint32_t ts_eax;	
  ...
	uint16_t ts_es;
	uint16_t ts_padding4;
	uint16_t ts_cs;
	...
};

```

<img src="./raw/lab3-7.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

##### **trapframe on stack**

<img src="./raw/lab3-8.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />



### why page fault not allowed in kernel?

> So you see, the safest (and simplest) solution is for the kernel to ensure that memory owned by the kernel is not pagable at all. For this reason, page faults should not really occur within the kernel. They can occur, but as @adobriyan notes, that usually indicates a much bigger error than a simple need to page in some memory. (I believe this is the case in Linux. Check your specific OS to be sure whether kernel memory is non-pagable. OS architectures do differ.)
>
> So in summary, kernel memory is usually not pagable, and since interrupts are usually handled within the kernel, page faults should not in general occur while servicing interrupts. Higher priority interrupts can still interrupt lower ones. It is just that all their resources are kept in physical memory.

[Page fault in Interrupt context](https://stackoverflow.com/questions/4848457/page-fault-in-interrupt-context)



### debug and breakpoint

- Fault—A program-state change does not accompany the debug exception, because the exception occurs before the faulting instruction is executed. The program can resume normal execution upon returning from the debug exception handler.
- Trap—A program-state change does accompany the debug exception, because the instruction or task switch being executed is allowed to complete before the exception is generated. However, the new state of the program is not corrupted and execution of the program can continue reliably

If the debugger sets the TF (trace flag), then every instruction will cause the debug (#1) interrupt to occur

<img src="./raw/lab3-9.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

[Why Single Stepping Instruction on X86?](https://stackoverflow.com/questions/7941988/why-single-stepping-instruction-on-x86)



### user-mode startup

user programs are first linked as part of kernel, then copied to user process later; 

xv6 is different on this, more valuable concerning memory & process management



### system call mechanism

```
static inline int32_t
syscall(int num, int check, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret;

	// Generic system call: pass system call number in AX,
	// up to five parameters in DX, CX, BX, DI, SI.
	// Interrupt kernel with T_SYSCALL.
	//
	// The "volatile" tells the assembler not to optimize
	// this instruction away just because we don't use the
	// return value.
	//
	// The last clause tells the assembler that this can
	// potentially change the condition codes and arbitrary
	// memory locations.

	asm volatile("int %1\n"
		     : "=a" (ret)
		     : "i" (T_SYSCALL),
		       "a" (num),
		       "d" (a1),
		       "c" (a2),
		       "b" (a3),
		       "D" (a4),
		       "S" (a5)
		     : "cc", "memory");
		     
	return ret;
}
```

- inc/syscall.h
- lib/syscall.c
- kern/syscall.*



## cpu alarm homework

ref [hw.md](hw.md) related section, as there are tricks on stacks upon interrupt privilege level change



## reference

**xv6** a simple, Unix-like teaching operating system: **Chapter 3 Traps, interrupts, and drivers**

**IA-32 3**: **chapter 5 interrupt and exception handling**



## knowledge

- elf executable format
- c compiler linker
