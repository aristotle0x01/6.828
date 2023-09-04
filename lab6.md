

## preparation reading

[Turtles on the wire: understanding how the OS uses the modern NIC](https://www.tritondatacenter.com/blog/virtualizing-nics) :  <font color="red">how os and NIC interact</font>

[Illustrated Guide to Monitoring and Tuning the Linux Networking Stack: Receiving Data](https://blog.packagecloud.io/illustrated-guide-monitoring-tuning-linux-networking-stack-receiving-data/)

<img src="./raw/lab6-nic-receive.jpeg?raw=true" alt="nic receive" style="zoom:25%; float:left" />

[qemu-networking](https://wiki.qemu.org/Documentation/Networking)

<img src="./raw/lab6-qemu-networking.png?raw=true" alt="nic receive" style="zoom:70%; float:left" />

[Linux networking stack from the ground up, part 1](https://www.privateinternetaccess.com/blog/linux-networking-stack-from-the-ground-up-part-1/) : on PCI init

Linux Network Receive Stack Monitoring and Tuning Deep Dive (Patrick Ladd, pdf): 

`lspci -nn`

<img src="./raw/lab6-lspci.jpg?raw=true" alt="nic receive" style="zoom:70%; float:left" />





## lab key points

- file system (on-disk structure)

- - sector and block: 512 -> 4096bytes

  - superblocks

  - file meta-data: struct File

  - directory vs regular file
  
    unlike xv6, no special inode blocks; the tree-like hierarchy has to be deduced from cache in memory
    
  
- disk access priviledge: **`ENV_TYPE_FS`; `IOPL_3`**

- block cache

  - **\#define DISKMAP  0x10000000**: disk block n mapped in memory
- diskaddr(uint32_t blockno)
  - flush_block

- file operations

  ***workhorses of the file system***

  - **file_block_walk**: maps from a block offset within a file to the pointer for that block in the `struct File`
  - **file_get_block**: maps to the actual disk block, allocating a new one if necessary

- file system interface

  - c/s mode

  - ipc mechanism

  - spawn process: **spawn vs exec**

  
  - state sharing between env through fork and spawn
    - **PTE_SHARE**:  ` serve_open: *perm_store = PTE_P|PTE_U|PTE_W|PTE_SHARE;`
    - *file descriptor* sharing: different env write to same file


- shell redirect: dup usage and its implementation tricks



## Questions



## knowledge

- network card
