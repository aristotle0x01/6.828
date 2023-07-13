

## answers-lab3

1. What is the purpose of having an individual handler function for each exception/interrupt? (i.e., if all exceptions/interrupts were delivered to the same handler, what feature that exists in the current implementation could not be provided?)

   Ans:  trapno wouldn't  be provided, thus you don't know what kind of interrupt acctually happened.

2. Did you have to do anything to make the `user/softint` program behave correctly? The grade script expects it to produce a general protection fault (trap 13), but `softint`'s code says `int $14`. *Why* should this produce interrupt vector 13? What happens if the kernel actually allows `softint`'s `int $14` instruction to invoke the kernel's page fault handler (which is interrupt vector 14)?

   Ans:  because its interrupt descriptor privilege level is ring 0, cann't be called from ring 3/user level



## **Exercises**

### **Exercise 9**

If you now run `user/breakpoint`, you should be able to run backtrace from the kernel monitor and see the backtrace traverse into `lib/libmain.c` before the kernel panics with a page fault. What causes this page fault? 

<img src="./raw/lab3-1.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

<img src="./raw/lab3-2.jpg?raw=true" alt="call convention" style="zoom:40%;float: left" />

<img src="./raw/lab3-3.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

Referenced user empty page, so page faulted







why page fault not allowed in kernel?

breakpoint



## skill sets

- make run-**faultread**-nox

  and the like without changing code in "ENV_CREATE(user_hello, ENV_TYPE_USER);"
  
- make grade

  not work locally, but you can inspect grade-lab3 by comparing outputs 

**knowledge**:

- BIOS
- ELF
- c calling convention: [BUFFER OVERFLOW 6 The Function Stack](https://www.tenouk.com/Bufferoverflowc/Bufferoverflow2a.html)
- i/o port development: [Chapter 6: Parallel I/O ports](https://users.ece.utexas.edu/~valvano/Volume1/E-Book/C6_MicrocontrollerPorts.htm)
- makefile
- C: K & R

