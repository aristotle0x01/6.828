

## lab key points

- part A: multi-cpu support and multitasking

  - processor

    - bootstrap processor
    - application processor
    - cpu initialization
      - kernel stack: `mem_init_mp()`
      - tss descriptor & registers
      - trap init: `trap_init_percpu()`
      - per cpu tss & gdt config
      - mpentry.S

  - locking: `lock_kernel(); unlock_kernel()`

    no more than one environment can run in kernel mode; lock is held when enter kernel mode, released when return to user mode

  - round-robin scheduling

    - sched_yield()
    - trap without trapret, unlike xv6

  - process management

    - sys_exofork
    - sys_env_set_status
    - sys_page_alloc
    - sys_page_map
    - sys_page_unmap

- part B: copy-on-write fork

  - user-level page fault handling
    - exception stacks in user env
      - sys_env_set_pgfault_upcall
      - _pgfault_upcall: lib/pfentry.S
      - set_pgfault_handler: lib/pgfault.c
      - stack arrangement & blank word
  - copy-on-write fork
    - uvpt & uvpd
    - PTE_COW & FEC_WR
    - lib/fork.c
      - fork()
      - duppage()
      - pgfault()

- part C: preemptive multitasking & IPC

  - preemption & clock interrupt
    - `FL_IF` flag disabled in kernel, enabled in user
    - clock generation: `lapicw(TIMER, PERIODIC | (IRQ_OFFSET + IRQ_TIMER))` 
  - IPC
    - send & recv msg
    - transferring pages
    - diff sys & lib functions



## **Exercises**

### **Exercise 2**

**Question**: the purpose of macro `MPBOOTPHYS` is to calculate the actual physical address, because kern/mpentry.S already linked above KERNBASE. It could go wrong if RELOC were omitted in `kern/mpentry.S`

- **link address**:  where in virtual space it would reside
- **load address**: where in physical space it would be loaded

### **Exercise 3**&4

is phy = PADDR(bootstack) still needed after adding mem_init_mp?

Ans: no, since in mem_init_mp its mapping will be removed by page_insert

### **Exercise 7**

user/dumbfork.c is pretty interesting:

a. **parent and child both mapped to the same physical page, thus memmove executed in parent actually effected mem in child**

```
void
duppage(envid_t dstenv, void *addr)
{
	int r;

	// This is NOT what you should do in your fork.
	if ((r = sys_page_alloc(dstenv, addr, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);
	if ((r = sys_page_map(dstenv, addr, 0, UTEMP, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_map: %e", r);
	memmove(UTEMP, addr, PGSIZE);
	if ((r = sys_page_unmap(0, UTEMP)) < 0)
		panic("sys_page_unmap: %e", r);
}
```

b. `duppage(envid, ROUNDDOWN(&addr, PGSIZE))` seems like xv6 user stack; while`duppage(envid, (void *)(USTACKTOP-PGSIZE))` use jos user stack. actually they are the same, since <u>addr</u> is a local var, then <u>&addr</u> will be its address, which is the stack memory address, further `ROUNDDOWN(&addr, PGSIZE)` would be equal to `(USTACKTOP-PGSIZE)`

<img src="./raw/xv6-user-stack.jpg?raw=true" alt="xv6" style="zoom:50%; float:left" />

<img src="./raw/jos-user-stack.jpg?raw=true" alt="jos" style="zoom:35%; float:left" />

```
envid_t
dumbfork(void)
{
	envid_t envid;
	uint8_t *addr;
	int r;
	extern unsigned char end[];
	...
	// Also copy the stack we are currently running on.
	// duppage(envid, ROUNDDOWN(&addr, PGSIZE));
	cprintf("stack %08x %08x %08x %08x\n", ROUNDDOWN(&addr, PGSIZE), USTACKTOP-PGSIZE, addr, end);
	duppage(envid, (void *)(USTACKTOP-PGSIZE));
	...
}
```

### **Exercise 8,9,10,11**

1.relations of set_pgfault_handler and sys_env_set_pgfault_upcall, kind of opaque

2.implementation of user space page fault handling

3.why a blank word on the user exception stack?

a.first stack overview

<img src="./raw/lab4-stack-overview.png?raw=true" alt="jos stack view" style="zoom:100%; float:left" />

b. stack arrangement & blank word

<img src="./raw/lab4-user-level-page-fault-exception.png?raw=true" alt="user exception stack" style="zoom:50%; float:left" />



### Exercise 12

how to interpret uvpt and uvpd

<img src="./raw/uvpt-interpretation.png?raw=true" alt="jos stack view" style="zoom:70%; float:left" />

implementation of copy-on-write

### Exercise 13 & 14
<font color="red">the syscall gate **istrap** caveat</font>

> In JOS, we make a key simplification compared to xv6 Unix. External device interrupts are *always* disabled when in the kernel (and, like xv6, enabled when in user space)

Influenced by xv6 syscall SETGATE, the istrap bit is set to **'1'**, but it is wrong here

```
SETGATE(idt[T_SYSCALL], 0, GD_KT, handler48, 3) // jos
SETGATE(idt[T_SYSCALL], 1, GD_KT, handler48, 3) // xv6
```

**simplification** 

```
// ref: gatsbyd/mit_6.828_jos_2018
env_setup_vm: 
	memcpy(e->env_pgdir, kern_pgdir, PGSIZE);

mem_init: 
	boot_map_region(kern_pgdir, KERNBASE, (uint32_t)(0xffffffff-KERNBASE+1), 0, PTE_W | PTE_P);
```


## reading comprehensions

### **memory ordering**

- **memory ordering**： The term refers to the order in which the processor issues reads (loads) and writes (stores) through the system bus to system memory

- **program ordering**： (generally referred to as **strong ordering**), where reads and writes are issued on the system bus in the order they occur in the instruction stream under all circumstances.

- **processor ordering**：These variations allow performance enhancing operations such as allowing reads to go ahead of buffered writes. The goal of any of these variations is to increase instruction execution speeds, while maintaining memory coherency, even in multiple-processor systems.

  ```
  __sync_synchronize();
  
  // in asm
  __sync_synchronize();
  8010436d:	f0 83 0c 24 00       	lock orl $0x0,(%esp)
  
  Overall, the instruction lock orl $0x0, (%esp) has no practical effect, as it performs a logical OR operation with a value of 0x0, which does not change the value stored in the memory location pointed to by esp. However, the lock prefix ensures that the operation is executed atomically, which can be useful in a multi-threaded or multi-processor environment where multiple threads or processors might be accessing the same memory location concurrently.
  ```

### concurrency may arise

- multiple cores

- multiple threads

- interrupt code

  **xv6 **a simple, Unix-like teaching operating system： chapter 4 locking [**Interrupt handlers**]

  > xv6 is more conservative: when a processor enters a spin-lock critical section, xv6 always ensures interrupts are disabled on that processor.Interrupts may still occur on other processors, so an interrupt’s acquire can wait for a thread to release a spin-lock; just not on the same processor
  >
  > ```
  > void
  > pushcli(void)
  > {
  > int eflags;
  > 
  > eflags = readeflags();
  > cli();
  > if(mycpu()->ncli == 0)
  >  mycpu()->intena = eflags & FL_IF;
  > mycpu()->ncli += 1;
  > }
  > 
  > void
  > popcli(void)
  > {
  > if(readeflags()&FL_IF)
  >  panic("popcli - interruptible");
  > if(--mycpu()->ncli < 0)
  >  panic("popcli");
  > if(mycpu()->ncli == 0 && mycpu()->intena)
  >  sti();
  > }
  > ```

### spin-lock vs sleep lock

sleep-locks leave interrupts enabled, they cannot be used in interrupt handlers.

### **Code: Scheduling** <!--worth reading-->

- xv6 acquires ptable.lock in one thread (often in yield) and releases the lock in a different thread (the scheduler thread or another next kernel thread): **<u>this is very interesting, that is to say, lock is maintained per cpu wide, not merely per thread wide</u>**
- when a new process is first scheduled, it begins at **<u>forkret</u>**. forkret exists to release the ptable.lock

<img src="./raw/xv6-scheduler-locking.png?raw=true" alt="ssh_port" style="zoom:60%;float: left" />

### **sleep and wakeup**

- classic consumer/producer problem
- xv6 implementation details and intricacies
- sleep: atomically release lock and sleep on chan, reacquires lock when awakened

```
// caller of sleep already hold "lk"
// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  ...
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}
```



## testing

 **is important, the way test is done in JOS**



## reference

**xv6** a simple, Unix-like teaching os: **chapter 4/5 Locking/Scheduling**

IA32-3: **CHAPTER 8 Advanced Programmable Interrupt Controller (APIC)**

IA32-3:  8.4 **LOCAL APIC**



## knowledge

- LAPIC
- python auto testing
