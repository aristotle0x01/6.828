

## lab1

### **main content**

- BIOS

  physical address arrangement

  BIOS real address 

  gdb trace

- bootloader

  enable protected mode

  read sector + load kernel elf

  objdump kernel elf

- kernel

  enable page table: virtual address translation

  console output: i/o development

  c stack calling convention



For detailed content notes,  ref: [6.828 Lecture Notes: x86 and PC architecture](https://pdos.csail.mit.edu/6.828/2018/lec/l-x86.html)

#### I/O ports

- Original PC architecture: use dedicated I/O space

  - Works same as memory accesses but set I/O signal

  - Only 1024 I/O addresses

  - Accessed with special instructions (IN, OUT)

    ```
    # Example: write a byte to line printer:
    #define DATA_PORT    0x378
    #define STATUS_PORT  0x379
    #define   BUSY 0x80
    #define CONTROL_PORT 0x37A
    #define   STROBE 0x01
    void
    lpt_putc(int c)
    {
      /* wait for printer to consume previous byte */
      while((inb(STATUS_PORT) & BUSY) == 0)
        ;
    
      /* put the byte on the parallel lines */
      outb(DATA_PORT, c);
    
      /* tell the printer to look at the data */
      outb(CONTROL_PORT, STROBE);
      outb(CONTROL_PORT, 0);
    }
    ```

- Memory-Mapped I/O

  - Use normal physical memory addresses
    - Gets around limited size of I/O address space
    - No need for special instructions
    - System controller routes to appropriate device
  - Works like ``magic'' memory:
    - *Addressed* and *accessed* like memory, but ...
    - ... does not *behave* like memory!
    - Reads and writes can have ``side effects''

ref: [Phil Storrs PC Hardware book:The more common I/O address assignments](http://web.archive.org/web/20040501054447/http://members.iweb.net.au/~pstorr/pcbook/book2/ioassign.htm) 

​       [The PC Parallel Ports](http://web.archive.org/web/20040501062046/http://members.iweb.net.au/~pstorr/pcbook/book2/parallel.htm) , [The PCs Serial Ports](http://web.archive.org/web/20040501062501/http://members.iweb.net.au/~pstorr/pcbook/book2/serial.htm) , [The PC Keyboard operation](http://web.archive.org/web/20040603193656/http://members.iweb.net.au/~pstorr/pcbook/book3/keyboard.htm)



#### gcc x86 calling conventions

- GCC dictates how the stack is used. Contract between caller and callee on x86:
  - at entry to a function (i.e. just after call):
    - %eip points at first instruction of function
    - %esp+4 points at first argument
    - %esp points at return address
  - after ret instruction:
    - %eip contains return address
    - %esp points at arguments pushed by caller
    - called function may have trashed arguments
    - %eax (and %edx, if return type is 64-bit) contains return value (or trash if function is `void`)
    - %eax, %edx (above), and %ecx may be trashed
    - %ebp, %ebx, %esi, %edi must contain contents from time of `call`
  - Terminology:
    - %eax, %ecx, %edx are "caller save" registers
    - %ebp, %ebx, %esi, %edi are "callee save" registers

Functions can do anything that doesn't violate contract. By convention, GCC does more:

- each function has a stack frame marked by %ebp, %esp

  ```
  		       +------------+   |
  		       | arg 2      |   \
  		       +------------+    >- previous function's stack frame
  		       | arg 1      |   /
  		       +------------+   |
  		       | ret %eip   |   /
  		       +============+   
  		       | saved %ebp |   \
  		%ebp-> +------------+   |
  		       |            |   |
  		       |   local    |   \
  		       | variables, |    >- current function's stack frame
  		       |    etc.    |   /
  		       |            |   |
  		       |            |   |
  		%esp-> +------------+   /
  ```

- %esp can move to make stack frame bigger, smaller

- %ebp points at saved %ebp from previous function, chain to walk stack

- function prologue:

  ```
  			pushl %ebp
  			movl %esp, %ebp
  ```

- function epilogue can easily find return EIP on stack:

  ```
  			movl %ebp, %esp
  			popl %ebp
  ```



### boot process

<img src="./raw/lab1-booting_process.png?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

**For boot process and hardware env**, ref: [xv6: a simple, Unix-like teaching operating system](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf). Appendix A:PC hardware; Appendix B: The boot loader



### skill set

**tool**:

- objdump
- gdb: CSCI0330 gdb Cheatsheet *Fall 2018*

**knowledge**:

- BIOS
- ELF
- c calling convention: [BUFFER OVERFLOW 6 The Function Stack](https://www.tenouk.com/Bufferoverflowc/Bufferoverflow2a.html)
- i/o port development: [Chapter 6: Parallel I/O ports](https://users.ece.utexas.edu/~valvano/Volume1/E-Book/C6_MicrocontrollerPorts.htm)
- makefile
- C: K & R



### Software Setup

```
git clone https://pdos.csail.mit.edu/6.828/2018/jos.git lab
with: SSL certificate problem: certificate has expired error => 

git -c http.sslVerify=false clone https://pdos.csail.mit.edu/6.828/2018/jos.git lab
```



### Exercises

**Exercise 7**

the first instruction failed would be `jmp *%eax `  (entry.S line 68: jmp 0xf010002c)

<img src="./raw/lab1-e7-1.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

<img src="./raw/lab1-e7-2.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />



**Exercise 9**

in entry.S line 77: `movl	$(bootstacktop),%esp`   =>  `mov    $0xf0110000,%esp`

Stack space is 0x10000 == 64k. Sit on the high end of memory.



**Exercise 10**

Three 32-bit words would each recursive nesting level of `test_backtrace` push on the stack: parameter, ret address, saved value of calling ebp.

<img src="./raw/lab1-e10-1.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

from: [The 32 bit x86 C Calling Convention](https://aaronbloomfield.github.io/pdr/book/x86-32bit-ccc-chapter.pdf)

With the image above, it is easy to understand: `add    $0x10,%esp`

```
	// Test the stack backtrace function (lab 1 only)
	test_backtrace(5);
f01000c8:	c7 04 24 05 00 00 00 	movl   $0x5,(%esp)
f01000cf:	e8 6c ff ff ff       	call   f0100040 <test_backtrace>
f01000d4:	83 c4 10             	add    $0x10,%esp
```

