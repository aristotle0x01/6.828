

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

<img src="./raw/lab3-1.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

<img src="./raw/lab3-2.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

<img src="./raw/lab3-3.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

Referenced user empty page, so page faulted



## Lab3

### user & kernel spaces

<img src="./raw/lab3-kernel-space.png?raw=true" alt="call convention" style="zoom:50%;float: left" />

### interrupt priviledge

#### **interrupt descriptor table (IDT)**

<img src="./raw/lab3-4.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

<img src="./raw/lab3-5.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

#### **Interrupt-Handler Procedures**

<img src="./raw/lab3-6.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />



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

<img src="./raw/lab3-7.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

##### **trapframe on stack**

<img src="./raw/lab3-8.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />







hw-cpu alarm

user env management

user-mode startup

why page fault not allowed in kernel?



### reference

**xv6** a simple, Unix-like teaching operating system: **Chapter 3 Traps, interrupts, and drivers**

**IA-32 3**: **chapter 5 interrupt and exception handling**

