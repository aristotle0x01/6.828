

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

### **Exercise 1**

Q1: no need, because it would be saved per env



## Questions

#### q1: in xv6, how does `filewrite & writei` ensure atomicity and durability or not?

Ans: from xv6 implementation, if `filewrite` was broken into several parts, then `writei` will only ensure consistency of each part



q2:  xv6 file system

<img src="./raw/xv6-file-system.png?raw=true" alt="xv6 file system" style="zoom:50%; float:left" />



## reference

[ATA PIO Mode](https://wiki.osdev.org/ATA_PIO_Mode)

[Parallel ATA](https://en.wikipedia.org/wiki/Parallel_ATA)



## knowledge

- storage dev
