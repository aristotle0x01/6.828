

## Lab2

### **File descriptors**

- xv6 kernel uses the file descriptor as an index into a per-process table, so that every process has a private space of file descriptors starting at zero
- Two file descriptors **share an offset** if they were derived from the same original file descriptor by a sequence of fork and dup calls
- A newly allocated file descriptor is always the lowest-numbered unused descriptor of the current process

**APIs**

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


