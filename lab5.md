

## lab key points

- key files

| `fs/fs.c`       | Code that mainipulates the file system's on-disk structure.  |
| --------------- | ------------------------------------------------------------ |
| `fs/bc.c`       | A simple block cache built on top of our user-level page fault handling facility. |
| `fs/ide.c`      | Minimal PIO-based (non-interrupt-driven) IDE driver code.    |
| `fs/serv.c`     | The file system server that interacts with client environments using file system IPCs. |
| `lib/fd.c`      | Code that implements the general UNIX-like file descriptor interface. |
| `lib/file.c`    | The driver for on-disk file type, implemented as a file system IPC client. |
| `lib/console.c` | The driver for console input/output file type.               |
| `lib/spawn.c`   | Code skeleton of the `spawn` library call.                   |

- file system (on-disk structure)

  - sector and block: 512 -> 4096bytes

  - superblocks

    ```
    struct Super {
    	uint32_t s_magic;		// Magic number: FS_MAGIC
    	uint32_t s_nblocks;		// Total number of blocks on disk
    	struct File s_root;		// Root directory node
    };
    ```

    <img src="./raw/lab5-disk.png?raw=true" alt="file" style="zoom:80%; float:left" />

  - file meta-data: struct File

    ```
    struct File {
    	char f_name[MAXNAMELEN];	// filename
    	off_t f_size;			// file size in bytes
    	uint32_t f_type;		// file type
    
    	// Block pointers.
    	// A block is allocated iff its value is != 0.
    	uint32_t f_direct[NDIRECT];	// direct blocks
    	uint32_t f_indirect;		// indirect block
    
    	// Pad out to 256 bytes; must do arithmetic in case we're compiling
    	// fsformat on a 64-bit machine.
    	uint8_t f_pad[256 - MAXNAMELEN - 8 - 4*NDIRECT - 4];
    } __attribute__((packed));	// required only on some 64-bit machines
    ```

    <img src="./raw/lab5-file.png?raw=true" alt="file" style="zoom:80%; float:left" />

  - directory vs regular file

    unlike xv6, no special inode blocks; the tree-like hierarchy has to be deduced from cache in memory
    
    <img src="./raw/lab5-file-hierarchy.png?raw=true" alt="file" style="zoom:80%; float:left" />

- disk access priviledge: **`ENV_TYPE_FS`; `IOPL_3`**

- block cache

  - **\#define DISKMAP  0x10000000**: disk block n mapped in memory

  - bc_pgfault: ***demand paging***

    ```
    void
    bc_init(void)
    {
    	struct Super super;
    	set_pgfault_handler(bc_pgfault); // file server
    	...
    }
    ```

  - diskaddr(uint32_t blockno)

  - flush_block

- file operations

  ***workhorses of the file system***

  - **file_block_walk**: maps from a block offset within a file to the pointer for that block in the `struct File`
  - **file_get_block**: maps to the actual disk block, allocating a new one if necessary

- file system interface

  - c/s mode

  - ipc mechanism

  - various structures & definitions: `union Fsipc`, `FDTABLE`, `struct Dev`, `struct OpenFile`

    ```
    Regular env           FS env
       +---------------+   +---------------+
       |      read     |   |   file_read   |
       |   (lib/fd.c)  |   |   (fs/fs.c)   |
    ...|.......|.......|...|.......^.......|...............
       |       v       |   |       |       | RPC mechanism
       |  devfile_read |   |  serve_read   |
       |  (lib/file.c) |   |  (fs/serv.c)  |
       |       |       |   |       ^       |
       |       v       |   |       |       |
       |     fsipc     |   |     serve     |
       |  (lib/file.c) |   |  (fs/serv.c)  |
       |       |       |   |       ^       |
       |       v       |   |       |       |
       |   ipc_send    |   |   ipc_recv    |
       |       |       |   |       ^       |
       +-------|-------+   +-------|-------+
               |                   |
               +-------------------+
    ```

- spawn process: **spawn vs exec**

  - state sharing between env through fork and spawn
    - **PTE_SHARE**:  ` serve_open: *perm_store = PTE_P|PTE_U|PTE_W|PTE_SHARE;`
    - *file descriptor* sharing: different env write to same file


- shell redirect: dup usage and its implementation tricks



## jos file ipc

<img src="./raw/lab5-jos-file-ipc.png?raw=true" alt="jos file system" style="zoom:70%; float:left" />



## Questions

#### q1: in xv6, how does `filewrite & writei` ensure atomicity and durability or not?

Ans: from xv6 implementation, if `filewrite` was broken into several parts, then `writei` will only ensure consistency of each part



q2:  xv6 file system

<img src="./raw/xv6-file-system.png?raw=true" alt="xv6 file system" style="zoom:50%; float:left" />



## ubuntu version issue

excercise 2 **check_super** won't pass on **ubuntu20**, changed to **ubuntu14-i386** it reported <u>**mmap /fs/clean-fs.img invalid argument**</u> error. Finally passed on **ubuntu-16.04.6-desktop-i386**



## reference

[ATA PIO Mode](https://wiki.osdev.org/ATA_PIO_Mode)

[Parallel ATA](https://en.wikipedia.org/wiki/Parallel_ATA)



## knowledge

- storage dev
