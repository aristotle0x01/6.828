### lazy page allocation

### phenomena

<img src="../raw/hw-lazy-1.jpg?raw=true" alt="page fault" style="zoom:50%;float: left" />

#### eip 0x104c 

**<u>xv6: sh.c</u>**

<img src="../raw/hw-lazy-2.jpg?raw=true" alt="shell" style="zoom:50%;float: left" />

<img src="../raw/hw-lazy-3.jpg?raw=true" alt="umalloc" style="zoom:35%;float: left" />



### xv6 CPU alarm

<img src="../raw/hw-cpu-alram.png?raw=true" alt="stack switch" style="zoom:35%;float: left" />



### xv6 locking

#### Don't do this

it would panic on the same cpu or block on another cpu



#### Interrupts in ide.c

double acquired lock



#### Interrupts in file.c

it relates to frequency



#### xv6 lock implementation

Why does `release()` clear `lk->pcs[0]` and `lk->cpu` *before* clearing `lk->locked`? Why not wait until after?

Ans: if `lk->locked` reset first in cpu A, at the same time if another cpu call `acquire` and get the lock in cpu B, then value of `lk->pcs; lk->cpu;` may be overrided by in A

