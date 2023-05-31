

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

<img src="./raw/lab1-booting_process.png?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

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

