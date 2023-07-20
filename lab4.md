

## **memory ordering**

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

  

## concurrency may arise

- multiple cores

- multiple threads

- interrupt code

  **xv6 **a simple, Unix-like teaching operating system： chapter 4 locking [**Interrupt handlers**]

  > Xv6 is more conservative: when a processor enters a spin-lock critical section, xv6 always ensures interrupts are disabled on that processor.Interrupts may still occur on other processors, so an interrupt’s acquire can wait for a thread to release a spin-lock; just not on the same processor
  >
  > ```
  > void
  > pushcli(void)
  > {
  >   int eflags;
  > 
  >   eflags = readeflags();
  >   cli();
  >   if(mycpu()->ncli == 0)
  >     mycpu()->intena = eflags & FL_IF;
  >   mycpu()->ncli += 1;
  > }
  > 
  > void
  > popcli(void)
  > {
  >   if(readeflags()&FL_IF)
  >     panic("popcli - interruptible");
  >   if(--mycpu()->ncli < 0)
  >     panic("popcli");
  >   if(mycpu()->ncli == 0 && mycpu()->intena)
  >     sti();
  > }
  > ```

### spin-lock vs sleep lock

sleep-locks leave interrupts enabled, they cannot be used in interrupt handlers.

### **Code: Scheduling** <!--worth reading-->

- xv6 acquires ptable.lock in one thread (often in yield) and releases the lock in a different thread (the scheduler thread or another next kernel thread): **<u>this is very interesting, that is to say, lock is maintained per cpu wide, not merely per thread wide</u>**
- when a new process is first scheduled, it begins at **<u>forkret</u>**. Forkret exists to release the ptable.lock

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



## **exercises**

### **Exercise 2**

**Question**: the purpose of macro `MPBOOTPHYS` is to calculate the actual physical address, because kern/mpentry.S already linked above KERNBASE. It could go wrong if RELOC were omitted in `kern/mpentry.S`

- **link address**:  where in virtual space it would reside
- **load address**: where in physical space it would be loaded



### **Exercise 3**&4

is phy = PADDR(bootstack) still needed after adding mem_init_mp?

Ans: no, since in mem_init_mp its mapping will be removed by page_insert

<img src="./raw/lab4-stack-overview.png?raw=true" alt="ssh_port" style="zoom:100%; float:left" />



## reference

**xv6** a simple, Unix-like teaching operating system: **Chapter 4/5 Locking/Scheduling**

**CHAPTER 8 ADVANCED PROGRAMMABLE INTERRUPT CONTROLLER (APIC)**

intel volume 3:  8.4 **LOCAL APIC**

**16.2.8.2.**



## knowledge

- A
- b
